#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include "gameObject.h"
#include "shotPool.h"

class ColliderManager
{
public:
    /**GameObject同士の衝突を判定 */
    void CheckCollisions(const std::vector<std::unique_ptr<GameObject>> &gameObjects);
    /**GameObjectと構造体の衝突を判定(ここではショット) */
    void CheckSOCollisions(const std::vector<std::unique_ptr<ShotPool>> &allShotPools, const std::vector<std::unique_ptr<GameObject>> &gameObjects);
};
