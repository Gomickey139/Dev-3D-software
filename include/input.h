#pragma once
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Input
{
private:
    static bool currentKeys[350];   // 現在のフレームでのキーの状態
    static bool previousKeys[350];  // 前のフレームでのキーの状態
    static glm::vec2 mousePosition; // 現在のマウス座標

public:
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods); // キー入力のコールバック関数
    static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);              // マウス座標のコールバック関数
    static void Update();

    static bool GetKey(int keyCode);     // キーが押されているかどうかを取得
    static bool GetKeyDown(int keyCode); // キーが押された瞬間かどうかを取得
    static bool GetKeyUp(int keyCode);   // キーが離された瞬間かどうかを取得

    static glm::vec2 GetMousePosition(); // マウス座標を取得
};