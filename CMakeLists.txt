cmake_minimum_required(VERSION 1.0)
project(opengl)

add_executable(opengl main.cpp)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})