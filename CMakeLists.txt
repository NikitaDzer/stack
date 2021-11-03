cmake_minimum_required(VERSION 3.20)
project(stack)

set(CMAKE_CXX_STANDARD 11)

add_executable(stack main.cpp include/stack.h src/dump.cpp include/dump.h include/config.h src/stack.cpp include/stack.h prototyping/prototyping.cpp prototyping/prototyping.h src/hash.cpp include/hash.h src/protect.cpp include/protect.h)
