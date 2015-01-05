cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

INCLUDE(FindPkgConfig)

pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
include_directories(${GTKMM_INCLUDE_DIRS} src)
link_directories(${GTKMM_LIBRARY_DIRS})

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

project(Fractal)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(fractal ${SRC_FILES})

target_link_libraries(fractal sfml-window sfml-system sfml-graphics ${GTKMM_LIBRARIES} pthread)
