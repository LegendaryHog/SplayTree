cmake_minimum_required(VERSION 3.8)

project(SplayTree LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(Threads REQUIRED)

find_package(GTest REQUIRED)
enable_testing()

include_directories(./include) 

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

add_subdirectory(unit-tests)
add_subdirectory(task)

