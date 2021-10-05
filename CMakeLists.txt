cmake_minimum_required(VERSION 3.20)
project(stack)

set(CMAKE_CXX_STANDARD 11)

add_executable(stack main.cpp include/stack.h src/stack_functions.cpp include/stack_functions.h)
