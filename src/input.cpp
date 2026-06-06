#include <iostream>

#include "input.h"

bool Input::currentKeys[350] = {false};
bool Input::previousKeys[350] = {false};

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