#pragma once

#include <glm/glm.hpp>

class Collider
{
public:
    // コライダーの中心のオフセット
    glm::vec3 offset;

    // コライダーの半径
    float radius;

    Collider(float radius, glm::vec3 offset = glm::vec3(0.0f));
};