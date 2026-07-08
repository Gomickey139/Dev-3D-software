#pragma once

#include <cstdint>
#include <random>

#include <glm/glm.hpp>

class RandomUtil
{
public:
    /** [min, max) の一様乱数を返す */
    static float Float(float min = 0.0f, float max = 1.0f);

    /** [min, max] の一様な整数乱数を返す */
    static int Int(int min, int max);

    /** 各成分が [min, max) の一様乱数であるベクトルを返す */
    static glm::vec3 Vec3(float min, float max);

    /** 単位球面上に一様分布する方向ベクトルを返す。 */
    static glm::vec3 UnitVector3();

    /** 再現可能な乱数列が必要な場合にシードを指定する。 */
    static void Seed(std::uint32_t seed);

private:
    static std::mt19937 &Engine();
};
