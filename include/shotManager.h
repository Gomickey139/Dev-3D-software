#pragma once

#include <memory>
#include <vector>

#include "shotPool.h"

/// 弾のプールを管理するクラス
class ShotManager
{
private:
    std::vector<std::unique_ptr<ShotPool>> m_pools;

public:
    ShotPool &AddPool(std::unique_ptr<ShotPool> pool);

    void Update(float deltaTime);
    void Draw(const glm::mat4 &view, const glm::mat4 &projection);
    void Clear();

    /**
     * @brief 弾のプールのリストを取得する
     * @return 弾のプールのリスト
     */
    const std::vector<std::unique_ptr<ShotPool>> &GetPools() const
    {
        return m_pools;
    }
};
