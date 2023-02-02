cmake_minimum_required(VERSION 1.0)
project(opengl)

add_executable(opengl main.cpp)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

target_link_libraries(opengl PRIVATE assimp::assimp)
target_link_libraries(opengl PRIVATE glfw)

target_sources(opengl PRIVATE glad.c)
target_include_directories(opengl PRIVATE glm)

include_directories(${OPENGL_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIR})