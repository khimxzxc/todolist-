#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Group.H>
#include <fstream>
#include <string>
#include <libgen.h>
#include <unistd.h>
#include <iostream>

Fl_Input* task_input;
Fl_Hold_Browser* task_list;
Fl_Button* add_btn;
Fl_Button* del_btn;
Fl_Group* main_group;

// Получение пути к папке с исполняемым файлом
std::string get_executable_dir(const char* argv0) {
#ifdef __APPLE__
    char path[1024];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        return std::string(dirname(path));
    }
    // Fallback: использовать argv[0]
    if (access(argv0, F_OK) == 0) {
        char fullpath[1024];
        realpath(argv0, fullpath);
        return std::string(dirname(fullpath));
    }
#endif
    return "."; // Текущая директория по умолчанию
}

// Определяем TASK_FILE как tasks.txt в папке с бинарником
static std::string task_file_path;
const char* TASK_FILE;

// Добавление задачи
void add_task(Fl_Widget*, void*) {
    const char* task = task_input->value();
    if (task && *task != '\0') {
        task_list->add(task);
        task_input->value("");
    }
}

// Удаление задачи
void delete_task(Fl_Widget*, void*) {
    int selected = task_list->value();
    if (selected > 0) {
        task_list->remove(selected);
    }
}

// Загрузка задач из файла
void load_tasks() {
    std::ifstream file(TASK_FILE);
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty())
            task_list->add(line.c_str());
    }
}

// Сохранение задач в файл
void save_tasks() {
    std::ofstream file(TASK_FILE);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть " << TASK_FILE << " для записи\n";
        return;
    }
    for (int i = 1; i <= task_list->size(); ++i) {
        const char* item = task_list->text(i);
        if (item) file << item << "\n";
    }
}

int main(int argc, char **argv) {
    // Устанавливаем путь к TASK_FILE на основе расположения бинарника
    task_file_path = get_executable_dir(argv[0]) + "/tasks.txt";
    TASK_FILE = task_file_path.c_str();

    Fl_Window* window = new Fl_Window(400, 300, "Todo List");

    main_group = new Fl_Group(0, 0, 400, 300);
    main_group->begin();

    task_input = new Fl_Input(60, 10, 230, 25, "Task:");
    task_input->align(FL_ALIGN_LEFT);

    add_btn = new Fl_Button(300, 10, 45, 25, "Add");
    add_btn->callback(add_task);

    del_btn = new Fl_Button(350, 10, 45, 25, "Delete");
    del_btn->callback(delete_task);

    task_list = new Fl_Hold_Browser(10, 50, 380, 240);
    main_group->resizable(task_list);
    main_group->end();

    window->resizable(main_group);

    load_tasks();                 // Загружаем при запуске
    atexit(save_tasks);          // Сохраняем при выходе

    window->end();
    window->show(argc, argv);
    return Fl::run();
}