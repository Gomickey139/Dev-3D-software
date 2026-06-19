
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "gameObject.h"

glm::mat4 Transform::GetModelMatrix() const
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

GameObject::GameObject(Mesh *mesh, Shader *shader, const std::string &name) : mesh(mesh), shader(shader), name(name) {};

void GameObject::AddChild(std::unique_ptr<GameObject> child)
{
    child->parent = this;
    children.push_back(std::move(child));
}

glm::mat4 GameObject::GetWorldMatrix() const
{
    glm::mat4 localMat = transform.GetModelMatrix();
    if (parent)
    {
        return parent->GetWorldMatrix() * localMat;
    }

    return localMat;
}

void GameObject::Init()
{
    for (auto &child : children)
    {
        child->Init();
    }
}

void GameObject::Update(float deltaTime)
{
    for (auto &child : children)
    {
        child->Update(deltaTime);
    }
}

void GameObject::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    if (mesh && shader)
    {
        shader->use();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        shader->setMat4("model", GetWorldMatrix());

        mesh->Draw();
    }

    for (auto &child : children)
    {
        child->Draw(view, projection);
    }
}

GameObject *GameObject::GetChildByName(const std::string &searchName)
{
    if (this->name == searchName)
    {
        return this;
    }

    for (auto &child : children)
    {
        GameObject *found = child->GetChildByName(searchName);
        if (found)
        {
            return found; // 見つかったら即座に返す
        }
    }

    return nullptr;
}

glm::vec3 GameObject::GetWorldColliderCenter()
{
    return transform.position + collider->offset;
}