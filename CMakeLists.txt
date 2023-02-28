cmake_minimum_required(VERSION 1.0)
project(opengl)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

set(IMGUI_DIR ../imgui)
set(CMAKE_CXX_STANDARD 11)

set(stb stb_image.h stb_image.cpp)
add_library(stb_lib ${stb})

add_executable(opengl ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)
add_executable(opengl main.cpp)

target_link_libraries(opengl PRIVATE assimp)
target_link_libraries(opengl PRIVATE glfw)
target_link_libraries(opengl PRIVATE stb_lib)
target_link_libraries(opengl PUBLIC glfw OpenGL::GL ${CMAKE_DL_LIBS})

target_sources(opengl PRIVATE glad.c)

target_include_directories(opengl PRIVATE glm)
target_include_directories(opengl PRIVATE imgui)

include_directories(${OPENGL_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIR} ${IMGUI_DIR} ${IMGUI_DIR}/backends)

file(GLOB fs "./*fs")
foreach(file ${fs})
    file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(GLOB vs "./*vs")
foreach(file ${vs})
    file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(GLOB materials "models/*.mtl")
foreach(file ${materials})
    file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(GLOB tex "models/*.png")
foreach(file ${tex})
    file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(GLOB tex "models/*.PNG")
foreach(file ${tex})
    file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(GLOB obj "models/*.obj")
foreach(file ${obj})
    file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(GLOB fbx "models/*.fbx")
foreach(file ${fbx})
    file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(GLOB dae "models/*.dae")
foreach(file ${dae})
    file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(COPY models/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
