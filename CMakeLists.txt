cmake_minimum_required(VERSION 3.20)
project(stack)

set(CMAKE_CXX_STANDARD 11)

add_executable(stack main.cpp src/stack.cpp include/stack.h)
