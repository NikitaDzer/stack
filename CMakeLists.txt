cmake_minimum_required(VERSION 3.20)
project(stack)

set(CMAKE_CXX_STANDARD 11)

add_executable(stack main.cpp include/stack.h src/stack_methods.cpp include/stack_methods.h src/dump.cpp include/dump.h src/config.cpp include/config.h src/stack.cpp include/stack.h prototyping/prototyping.cpp prototyping/prototyping.h)
