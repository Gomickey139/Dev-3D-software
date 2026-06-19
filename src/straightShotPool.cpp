#include "straightShotPool.h"

StraightShotPool::StraightShotPool(std::string name, int maxBullets, Mesh *mesh, Shader *shader, float fireRate, float speed, float lifeTime, float collisionRadius, glm::vec3 scale)
    : ShotPool(name, maxBullets, mesh, shader, fireRate, speed, lifeTime, collisionRadius, scale) {}

void StraightShotPool::UpdateMovement(float deltaTime)
{
    for (auto &s : m_shots)
    {
        if (s.isActive)
        {
            s.position += s.velocity * deltaTime;
        }
    }
}