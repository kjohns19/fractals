cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(example)

INCLUDE(FindPkgConfig)

pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})

file(GLOB_RECURSE SRC_FILES_example src/*.cpp)
add_executable(example ${SRC_FILES_example})
target_compile_options(example PRIVATE -std=c++14 -g -Wall -Werror)
target_include_directories(example PRIVATE ${GTKMM_INCLUDE_DIRS} src)
target_link_libraries(example ${GTKMM_LIBRARIES} sfml-system sfml-window sfml-graphics kj)

