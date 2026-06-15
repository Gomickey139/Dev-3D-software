#include <iostream>

#include "input.h"

bool Input::currentKeys[350] = {false};
bool Input::previousKeys[350] = {false};

glm::vec2 Input::mousePosition = glm::vec2(0.0f);

void Input::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 350)
    {
        if (action == GLFW_PRESS)
        {
            currentKeys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            currentKeys[key] = false;
        }
    }
}

void Input::CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    // double から float に変換して vec2 に格納
    mousePosition.x = static_cast<float>(xpos);
    mousePosition.y = static_cast<float>(ypos);
}

void Input::Update()
{
    // 全キーの状態を更新
    for (int i = 0; i < 350; i++)
    {
        // 現在の状態を「1フレーム前の状態」として保存
        previousKeys[i] = currentKeys[i];
    }
}

bool Input::GetKey(int keyCode)
{
    if (keyCode < 0 || keyCode >= 350)
        return false;
    return currentKeys[keyCode];
}

bool Input::GetKeyDown(int keyCode)
{
    if (keyCode < 0 || keyCode >= 350)
        return false;

    return currentKeys[keyCode] && !previousKeys[keyCode];
}

bool Input::GetKeyUp(int keyCode)
{
    if (keyCode < 0 || keyCode >= 350)
        return false;

    return !currentKeys[keyCode] && previousKeys[keyCode];
}

glm::vec2 Input::GetMousePosition()
{
    return mousePosition;
}