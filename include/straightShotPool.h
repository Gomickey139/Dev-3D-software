#pragma once
#include "shotPool.h"

class StraightShotPool : public ShotPool
{
public:
    StraightShotPool(std::string name, int maxBullets, Mesh *mesh, Shader *shader, float fireRate, float speed, float lifeTime, float collisionRadius, glm::vec3 scale = glm::vec3(0.5f));

    void UpdateMovement(float deltaTime) override;
};