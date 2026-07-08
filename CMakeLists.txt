cmake_minimum_required(VERSION 3.15)
project(RaylibEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(zero src/main.cpp)

target_link_libraries(zero PRIVATE raylib)
