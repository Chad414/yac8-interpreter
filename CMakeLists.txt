cmake_minimum_required(VERSION 3.15)
project(yac8_interpreter)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

add_executable(yac8_interpreter src/main.cpp src/Disassembler.cpp include/Disassembler.h src/CHIP-8.cpp)

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::Main)