#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "shader.h"

struct Transform
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 getModelMatrix() const;
};

class GameObject
{
public:
    Transform transform;
    Mesh *mesh;
    Shader *shader;
    GameObject(Mesh *mesh, Shader *shader);

    void Draw(const glm::mat4 &view, const glm::mat4 &projection);
};