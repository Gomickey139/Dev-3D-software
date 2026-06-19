#pragma once
#include <memory>
#include "shotPool.h"

class Weapon
{
private:
    std::unique_ptr<ShotPool> m_pool;

public:
    Weapon(std::unique_ptr<ShotPool> pool);

    void UpdateAndFire(float deltaTime, bool isFiring, const std::vector<glm::vec3> &pos, glm::vec3 dir);

    void Draw(const glm::mat4 &view, const glm::mat4 &projection);
};
