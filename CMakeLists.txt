cmake_minimum_required(VERSION 3.10)
project(hw14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(hw14 main.cpp "searching.h" "searching.cpp")