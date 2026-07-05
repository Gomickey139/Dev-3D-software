#include "weapon_spreadShot.h"

#include <cmath>
#include <glm/gtc/constants.hpp>

void Weapon_SpreadShot::Fire(const glm::vec3 &pos, const glm::vec3 &dir, float radius, int count)
{
    constexpr float MIN_DIRECTION_LENGTH_SQUARED = 1.0e-8f;

    if (count <= 0 || radius < 0.0f || glm::dot(dir, dir) <= MIN_DIRECTION_LENGTH_SQUARED)
    {
        return;
    }

    const glm::vec3 forward = glm::normalize(dir);

    // forwardをローカルZ軸とみなし、それに垂直なXY平面の基底を作る。
    const glm::vec3 reference =
        std::abs(forward.z) < 0.999f
            ? glm::vec3(0.0f, 0.0f, 1.0f)
            : glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 axisX = glm::normalize(glm::cross(reference, forward));
    const glm::vec3 axisY = glm::cross(forward, axisX);

    std::uniform_real_distribution<float> unitDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> signedUnitDistribution(-1.0f, 1.0f);
    std::uniform_real_distribution<float> angularSpeedDistribution(1.0f, 3.0f);
    std::vector<ShotSpawnParams> shots;
    shots.reserve(static_cast<std::size_t>(count));

    for (int i = 0; i < count; ++i)
    {
        const float distance = radius * std::sqrt(unitDistribution(m_randomEngine));
        const float angle = glm::two_pi<float>() * unitDistribution(m_randomEngine);
        const glm::vec3 offset =
            axisX * (distance * std::cos(angle)) +
            axisY * (distance * std::sin(angle));

        // 初期角度と回転軸を弾ごとに独立して決める。
        const glm::vec3 initialRotation(
            glm::two_pi<float>() * unitDistribution(m_randomEngine),
            glm::two_pi<float>() * unitDistribution(m_randomEngine),
            glm::two_pi<float>() * unitDistribution(m_randomEngine));

        const float axisZ = signedUnitDistribution(m_randomEngine);
        const float axisAngle = glm::two_pi<float>() * unitDistribution(m_randomEngine);
        const float axisRadius = std::sqrt(1.0f - axisZ * axisZ);
        const glm::vec3 rotationAxis(
            axisRadius * std::cos(axisAngle),
            axisRadius * std::sin(axisAngle),
            axisZ);

        ShotSpawnParams params;
        params.position = pos + offset;
        params.velocity = forward * GetShotSpeed();
        params.rotation = initialRotation;
        params.angularVelocity =
            rotationAxis * angularSpeedDistribution(m_randomEngine);
        shots.push_back(params);
    }

    // 発射間隔とShotPoolへの登録は基底Weaponに任せる。
    FirePreparedShots(shots);
}

void Weapon_SpreadShot::Update(float deltaTime)
{
    Weapon::Update(deltaTime);
}
