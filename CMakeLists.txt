cmake_minimum_required(VERSION 3.15)
project(yac8_interpreter)

set(CMAKE_CXX_STANDARD 14)

add_executable(yac8_interpreter src/main.cpp src/Disassembler.cpp include/Disassembler.h)