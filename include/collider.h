#pragma once

#include <glm/glm.hpp>

class Collider
{
public:
    glm::vec3 offset;

    float radius;

    Collider(float radius, glm::vec3 offset = glm::vec3(0.0f));
};