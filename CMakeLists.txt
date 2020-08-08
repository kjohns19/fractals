cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(fractal)

INCLUDE(FindPkgConfig)

pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})

file(GLOB_RECURSE SRC_FILES_fractal src/*cpp)
add_executable(fractal ${SRC_FILES_fractal})
target_compile_options(fractal PRIVATE -std=c++17 -g -O3 -Wall -Werror)
target_include_directories(fractal PRIVATE ${GTKMM_INCLUDE_DIRS} src)
target_link_libraries(fractal sfml-system sfml-window sfml-graphics ${GTKMM_LIBRARIES} pthread)

