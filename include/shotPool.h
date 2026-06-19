#pragma once

#include <vector>
#include "mesh.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Shot
{
    glm::vec3 position;
    glm::vec3 velocity;
    float radius;
    float lifeTime;
    bool isActive = false;
};

class ShotPool
{
protected:
    std::vector<Shot> m_shots;

    Mesh *m_sharedMesh;
    Shader *m_sharedShader;

    float m_fireRate;
    float m_fireTimer = 0.0f;

    float m_shotSpeed;
    float m_shotLifeTime;
    float m_collisionRadius;
    glm::vec3 m_shotScale;

    std::string m_name;

    void Fire(glm::vec3 pos, glm::vec3 dir);

    virtual void UpdateMovement(float deltaTime) = 0;

public:
    ShotPool(std::string name, int maxBullets, Mesh *mesh, Shader *shader, float fireRate, float speed, float lifeTime, float collisionRadius, glm::vec3 scale = glm::vec3(0.5f));

    virtual ~ShotPool() = default;

    std::string GetName() { return m_name; }

    void UpdateAndFire(float deltaTime, bool isFireButtonPressed, const std::vector<glm::vec3> spawnPos, glm::vec3 direction);

    void Draw(const glm::mat4 &view, const glm::mat4 &projection);

    std::vector<Shot> &GetShots() { return m_shots; }

    float GetCollisionRadius() const { return m_collisionRadius; }

    void Collision(std::string name, Shot &s);
};
