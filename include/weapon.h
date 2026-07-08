#pragma once
#include <memory>
#include "shotPool.h"
#include "randomUtil.h"

class Weapon
{
private:
    ShotPool &m_pool;
    float m_fireRate; // 発射間隔
    float m_fireTimer = 0.0f;
    float m_shotSpeed;
    float m_size = 0.5f;

protected:
    bool
    FirePreparedShots(const std::vector<ShotSpawnParams> &shots);
    float GetShotSpeed() const { return m_shotSpeed; }
    float GetSize() const { return m_size; }

public:
    Weapon(ShotPool &pool, float fireRate, float speed, float size = 0.5f);
    virtual ~Weapon() = default;

    virtual void Fire(const std::vector<glm::vec3> &pos, const std::vector<glm::vec3> &dir, bool isRandomRotation = false);

    virtual void Fire(const glm::vec3 &pos, const glm::vec3 &dir, float radius, int count) {}

    virtual void Update(float deltaTime);
};
