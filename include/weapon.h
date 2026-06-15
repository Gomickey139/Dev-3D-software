#pragma once
#include <memory>
#include "shotPool.h"

class Weapon
{
private:
    std::unique_ptr<ShotPool> m_pool;

public:
    Weapon(Mesh *shotMesh, Shader *shotShader, float fireRate, int maxShots = 100);

    void UpdateAndFire(float deltaTime, bool isFiring, glm::vec3 pos, glm::vec3 dir);
    void UpdateAndFire(float deltaTime, bool isFiring, std::array<glm::vec3, 2> pos, glm::vec3 dir);

    void Draw(const glm::mat4 &view, const glm::mat4 &projection);
};
