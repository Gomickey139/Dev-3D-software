#include "weapon.h"

Weapon::Weapon(std::unique_ptr<ShotPool> pool)
{
    m_pool = std::move(pool);
}

void Weapon::UpdateAndFire(float deltaTime, bool isFiring, const std::vector<glm::vec3> &pos, glm::vec3 dir)
{
    m_pool->UpdateAndFire(deltaTime, isFiring, pos, dir);
}

void Weapon::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    m_pool->Draw(view, projection);
}