cmake_minimum_required(VERSION 3.10)
project(My3DApp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 実行ファイル名
add_executable(My3DApp src/main.cpp)

# ライブラリの検索
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# リンク設定
target_link_libraries(My3DApp glfw OpenGL::GL)