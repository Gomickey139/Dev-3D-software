#pragma once

#include <vector>
#include <array>
#include "mesh.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Shot
{
    glm::vec3 position;
    glm::vec3 velocity;
    float lifeTime;
    bool isActive = false;
};

class ShotPool
{
private:
    std::vector<Shot> m_shots;

    Mesh *m_sharedMesh;
    Shader *m_sharedShader;

    float m_fireRate;
    float m_fireTimer = 0.0f;

    void Fire(glm::vec3 pos, glm::vec3 dir);

public:
    ShotPool(int maxBullets, Mesh *mesh, Shader *shader, float fireRate = 0.1f);

    void UpdateAndFire(float deltaTime, bool isFireButtonPressed, std::array<glm::vec3, 2> spawnPos, glm::vec3 direction);

    void Draw(const glm::mat4 &view, const glm::mat4 &projection);
};
