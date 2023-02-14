cmake_minimum_required(VERSION 1.0)
project(opengl)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

set(IMGUI_DIR ../imgui)

set(stb stb_image.h stb_image.cpp)
add_library(stb_lib ${stb})

add_executable(opengl ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)
add_executable(opengl main.cpp)

target_link_libraries(opengl PRIVATE assimp::assimp)
target_link_libraries(opengl PRIVATE glfw)
target_link_libraries(opengl PRIVATE stb_lib)

target_sources(opengl PRIVATE glad.c)

target_include_directories(opengl PRIVATE glm)
target_include_directories(opengl PRIVATE imgui)

include_directories(${OPENGL_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIR} ${IMGUI_DIR} ${IMGUI_DIR}/backends)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fshader.fs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/vshader.vs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/wall.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/container.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Darunia.obj DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
