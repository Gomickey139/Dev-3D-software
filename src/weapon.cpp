#include "weapon.h"

Weapon::Weapon(Mesh *shotMesh, Shader *shotShader, float fireRate, int maxShots)
{
    m_pool = std::make_unique<ShotPool>(maxShots, shotMesh, shotShader, fireRate);
}

void Weapon::UpdateAndFire(float deltaTime, bool isFiring, glm::vec3 pos, glm::vec3 dir)
{
}
void Weapon::UpdateAndFire(float deltaTime, bool isFiring, std::array<glm::vec3, 2> pos, glm::vec3 dir)
{
    m_pool->UpdateAndFire(deltaTime, isFiring, pos, dir);
}

void Weapon::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    m_pool->Draw(view, projection);
}