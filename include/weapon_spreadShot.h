#pragma once
#include <random>

#include "weapon.h"

class Weapon_SpreadShot : public Weapon
{
private:
    std::mt19937 m_randomEngine{std::random_device{}()};

public:
    Weapon_SpreadShot(std::unique_ptr<ShotPool> pool, float fireRate, float speed)
        : Weapon(std::move(pool), fireRate, speed) {}

    /**
     * @brief 指定した位置から指定した方向に、指定した半径で複数の弾を発射する
     * @param pos 発射位置
     * @param dir 発射方向
     * @param radius 発射する弾の広がりの半径
     * @param count 発射する弾の数
     */
    void Fire(const glm::vec3 &pos, const glm::vec3 &dir, float radius, int count) override;

    void Update(float deltaTime) override;
};
