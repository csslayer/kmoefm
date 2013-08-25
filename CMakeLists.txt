project(moefm)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(KDE4 REQUIRED)
find_package(QOAuth REQUIRED)
find_package(QCA2 REQUIRED)
find_package(QJson REQUIRED)
find_package(Phonon REQUIRED)

add_subdirectory(src)

add_subdirectory(po)

add_subdirectory(icons)
