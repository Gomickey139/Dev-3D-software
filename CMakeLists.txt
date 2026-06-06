cmake_minimum_required(VERSION 3.10)
project(My3DApp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 実行ファイル名
add_executable(My3DApp src/main.cpp src/gameObject.cpp src/mesh.cpp src/shader.cpp src/input.cpp src/player.cpp)

target_include_directories(My3DApp PRIVATE include)

set(OpenGL_GL_PREFERENCE GLVND)

# ライブラリの検索
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# リンク設定
target_link_libraries(My3DApp glfw OpenGL::GL)