#include "randomUtil.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <glm/gtc/constants.hpp>

std::mt19937 &RandomUtil::Engine()
{
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

float RandomUtil::Float(float min, float max)
{
    if (min > max)
    {
        throw std::invalid_argument("RandomUtil::Float requires min <= max");
    }

    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(Engine());
}

int RandomUtil::Int(int min, int max)
{
    if (min > max)
    {
        throw std::invalid_argument("RandomUtil::Int requires min <= max");
    }

    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(Engine());
}

glm::vec3 RandomUtil::Vec3(float min, float max)
{
    return glm::vec3(
        Float(min, max),
        Float(min, max),
        Float(min, max));
}

glm::vec3 RandomUtil::UnitVector3()
{
    const float z = Float(-1.0f, 1.0f);
    const float angle = Float(0.0f, glm::two_pi<float>());
    const float radius = std::sqrt(std::max(0.0f, 1.0f - z * z));

    return glm::vec3(
        radius * std::cos(angle),
        radius * std::sin(angle),
        z);
}

void RandomUtil::Seed(std::uint32_t seed)
{
    Engine().seed(seed);
}
