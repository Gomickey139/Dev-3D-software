#pragma once
#include <GLFW/glfw3.h>

class Input
{
private:
    static bool currentKeys[350];
    static bool previousKeys[350];

public:
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void Update();

    static bool GetKey(int keyCode);
    static bool GetKeyDown(int keyCode);
    static bool GetKeyUp(int keyCode);
};