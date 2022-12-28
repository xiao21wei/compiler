cmake_minimum_required(VERSION 3.22)
project(_compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(_compiler main.cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
