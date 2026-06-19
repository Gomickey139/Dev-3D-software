#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <string>

#include "mesh.h"
#include "shader.h"
#include "collider.h"

struct Transform
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 GetModelMatrix() const;
};

class GameObject
{
public:
    Transform transform;
    Mesh *mesh;
    Shader *shader;
    std::string name;

    GameObject *parent = nullptr;
    std::vector<std::unique_ptr<GameObject>> children;

    std::unique_ptr<Collider> collider;

    GameObject(Mesh *mesh, Shader *shader, const std::string &name = "GameObject");
    virtual ~GameObject() = default;

    void AddChild(std::unique_ptr<GameObject> child);

    glm::mat4 GetWorldMatrix() const;

    virtual void Init();

    virtual void Update(float deltatime);

    virtual void Draw(const glm::mat4 &view, const glm::mat4 &projection);

    virtual void Collision(std::string name) {}

    /**その名前の子オブジェクトのポインタを返す */
    GameObject *GetChildByName(const std::string &searchName);

    /**コライダーの中心のワールド座標を返す */
    glm::vec3 GetWorldColliderCenter();
};