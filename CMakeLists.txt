cmake_minimum_required(VERSION 3.14)
project(My3DApp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 実行ファイル名
add_executable(My3DApp 
    src/main.cpp #
    src/glad.c
    src/gameObject.cpp 
    src/mesh.cpp 
    src/shader.cpp 
    src/input.cpp 
    src/player.cpp 
    src/enemy.cpp 
    src/application.cpp 
    src/shotPool.cpp 
    src/weapon.cpp
    src/collider.cpp
    src/collisionMaganer.cpp
    src/straightShotPool.cpp
)

target_include_directories(My3DApp PRIVATE include)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8 # 安定版のバージョン
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8 # 安定版
)
# GLMの不要なテストビルドをスキップ
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

set(OpenGL_GL_PREFERENCE GLVND)

# ライブラリの検索
find_package(OpenGL REQUIRED)

# リンク設定
target_link_libraries(My3DApp glfw OpenGL::GL glm)
if(WIN32)
    target_link_options(My3DApp PRIVATE -static -static-libgcc -static-libstdc++)
endif()