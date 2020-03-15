cmake_minimum_required(VERSION 3.15)
project(yac8_interpreter)

set(CMAKE_CXX_STANDARD 14)


add_executable(yac8_interpreter src/main.cpp src/Disassembler.cpp include/Disassembler.h src/CHIP-8.cpp src/Display.cpp include/Display.h 
    include/SimpleRender/BufferData.cpp include/SimpleRender/BufferData.h
    include/SimpleRender/SimpleRender.cpp include/SimpleRender/SimpleRender.h)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)


target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
