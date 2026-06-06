#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "gameObject.h"

glm::mat4 Transform::getModelMatrix() const
{
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, position);
    // 回転 (Y -> X -> Z の順)
    m = glm::rotate(m, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::rotate(m, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::rotate(m, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    m = glm::scale(m, scale);
    return m;
}

GameObject::GameObject(Mesh *mesh, Shader *shader) : mesh(mesh), shader(shader) {};

void GameObject::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    if (!mesh || !shader)
        return;

    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", transform.getModelMatrix());

    mesh->Draw();
}
