cmake_minimum_required(VERSION 3.10)
project(todolist)

set(CMAKE_CXX_STANDARD 20)

find_package(FLTK REQUIRED NO_MODULE)
# Добавляем исполнимый файл
add_executable(todolist main.cpp)

# Находим FLTK
find_package(FLTK REQUIRED)

# Линкуем с FLTK
target_link_libraries(todolist PRIVATE fltk)
