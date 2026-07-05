#include "weapon.h"

#include <algorithm>

Weapon::Weapon(std::unique_ptr<ShotPool> pool, float fireRate, float speed)
    : m_fireRate(fireRate), m_shotSpeed(speed)
{
    m_pool = std::move(pool);
}

void Weapon::Update(float deltaTime)
{
    m_fireTimer = std::max(0.0f, m_fireTimer - deltaTime);
    m_pool->Update(deltaTime);
}

void Weapon::Fire(const std::vector<glm::vec3> &pos, const std::vector<glm::vec3> &dir)
{
    std::vector<ShotSpawnParams> shots;
    shots.reserve(std::min(pos.size(), dir.size()));

    for (std::size_t i = 0; i < pos.size() && i < dir.size(); ++i)
    {
        ShotSpawnParams params;
        params.velocity = glm::normalize(dir[i]) * m_shotSpeed;
        params.position = pos[i];
        shots.push_back(params);
    }

    FirePreparedShots(shots);
}

bool Weapon::FirePreparedShots(const std::vector<ShotSpawnParams> &shots)
{
    if (m_fireTimer > 0.0f || shots.empty())
    {
        return false;
    }

    for (const auto &shot : shots)
    {
        m_pool->Spawn(shot);
    }

    m_fireTimer = m_fireRate;
    return true;
}

void Weapon::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    m_pool->Draw(view, projection);
}
