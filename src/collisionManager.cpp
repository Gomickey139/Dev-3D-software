#include "collisionManager.h"

void ColliderManager::CheckCollisions(const std::vector<std::unique_ptr<GameObject>> &gameObjects)
{
    for (int i = 0; i < gameObjects.size(); i++)
    {
        auto &obj1 = gameObjects[i];
        if (!obj1->collider)
        {
            continue;
        }

        for (int j = 0; j < gameObjects.size(); j++)
        {
            auto &obj2 = gameObjects[j];
            if (!obj2->collider || i == j)
            {
                continue;
            }

            float dist = glm::distance(obj1->GetWorldColliderCenter(), obj2->GetWorldColliderCenter());
            float minDist = obj1->collider->radius + obj2->collider->radius;

            // std::cout << "(" << obj1->transform.position.x << "," << obj1->transform.position.y << "," << obj1->transform.position.z << ")と(" << obj2->transform.position.x << "," << obj2->transform.position.y << "," << obj2->transform.position.z << ")" << std::endl;
            if (dist < minDist)
            {
                std::cout << obj1->name << "と" << obj2->name << "が衝突しました" << std::endl;
                obj1->Collision(obj2->name);
                obj2->Collision(obj1->name);
            }
        }
    }
}

void ColliderManager::CheckSOCollisions(const std::vector<std::unique_ptr<ShotPool>> &allShotPools, const std::vector<std::unique_ptr<GameObject>> &gameObjects)
{
    for (const auto &pool : allShotPools)
    {
        float currentColligionRadius = pool->GetCollisionRadius();

        for (auto &shot : pool->GetShots())
        {
            if (!shot.isActive)
            {
                continue;
            }

            for (auto &obj : gameObjects)
            {
                if (!obj->collider)
                {
                    continue;
                }

                float dist = glm::distance(shot.position, obj->GetWorldColliderCenter());

                float minDist = currentColligionRadius + obj->collider->radius;

                if (dist < minDist)
                {

                    // std::cout << pool->GetName() << "と" << obj->name << "が衝突しました" << std::endl;
                    obj->Collision(pool->GetName());
                    pool->Collision(obj->name, shot);
                }
            }
        }
    }
}
