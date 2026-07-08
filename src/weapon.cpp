#include "weapon.h"

#include <algorithm>

Weapon::Weapon(ShotPool &pool, float fireRate, float speed, float size)
    : m_pool(pool), m_fireRate(fireRate), m_shotSpeed(speed), m_size(size) {}

void Weapon::Update(float deltaTime)
{
    m_fireTimer = std::max(0.0f, m_fireTimer - deltaTime);
}

void Weapon::Fire(const std::vector<glm::vec3> &pos, const std::vector<glm::vec3> &dir, bool isRandomRotation)
{
    std::vector<ShotSpawnParams> shots;
    shots.reserve(std::min(pos.size(), dir.size()));

    for (std::size_t i = 0; i < pos.size() && i < dir.size(); ++i)
    {
        ShotSpawnParams params;
        params.velocity = glm::normalize(dir[i]) * m_shotSpeed;
        params.position = pos[i];
        params.scale = glm::vec3(m_size);
        if (isRandomRotation)
        {
            params.rotation = RandomUtil::Vec3(0.0f, glm::two_pi<float>());
            params.angularVelocity = RandomUtil::UnitVector3() * RandomUtil::Float(1.0f, 3.0f);
        }
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
        m_pool.Spawn(shot);
    }

    m_fireTimer = m_fireRate;
    return true;
}
