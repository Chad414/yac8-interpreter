cmake_minimum_required(VERSION 3.15)
project(yac8_interpreter)

# C++ Standard Version
set(CMAKE_CXX_STANDARD 14)

# Get Build Type
if(NOT CMAKE_BUILD_TYPE)            # Default = Release
    set(CMAKE_BUILD_TYPE Release)
endif()

# Congiure C++ Flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra")    # General C-Flags
set(CMAKE_CXX_FLAGS_DEBUG "-g")         # Debug Compilation
set(CMAKE_CXX_FLAGS_RELEASE "-O3")      # Best Optimization


add_executable(yac8_interpreter src/main.cpp src/Disassembler.cpp include/Disassembler.h src/CHIP-8.cpp src/Display.cpp include/Display.h 
    include/SimpleRender/BufferData.cpp include/SimpleRender/BufferData.h
    include/SimpleRender/SimpleRender.cpp include/SimpleRender/SimpleRender.h)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)


target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
