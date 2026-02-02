cmake_minimum_required(VERSION 3.20)
project(engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/inc)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_compile_definitions(GLEW_STATIC)

add_executable(engine
    src/main.cpp

    src/GLA/buffer.cpp
    src/GLA/debug.cpp
    src/GLA/program.cpp
    src/GLA/shader.cpp
    src/GLA/windowContext.cpp
    src/GLA/vertexArray.cpp
)

add_compile_definitions(DEBUG_BUILD) # define DEBUG_BUILD for GL_CALL error (slows down the program in release)

target_link_libraries(engine glfw3 opengl32 glew32s)