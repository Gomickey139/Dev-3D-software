#include "shotManager.h"

#include <stdexcept>

ShotPool &ShotManager::AddPool(std::unique_ptr<ShotPool> pool)
{
    if (!pool)
    {
        throw std::invalid_argument("ShotManager::AddPool received a null pool");
    }

    ShotPool &poolRef = *pool;
    m_pools.push_back(std::move(pool));
    return poolRef;
}

void ShotManager::Update(float deltaTime)
{
    for (auto &pool : m_pools)
    {
        pool->Update(deltaTime);
    }
}

void ShotManager::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    for (auto &pool : m_pools)
    {
        pool->Draw(view, projection);
    }
}

void ShotManager::Clear()
{
    m_pools.clear();
}
