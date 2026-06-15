#pragma once
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Input
{
private:
    static bool currentKeys[350];
    static bool previousKeys[350];
    static glm::vec2 mousePosition;

public:
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);
    static void Update();

    static bool GetKey(int keyCode);
    static bool GetKeyDown(int keyCode);
    static bool GetKeyUp(int keyCode);

    static glm::vec2 GetMousePosition();
};