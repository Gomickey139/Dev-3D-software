#include "enemy.h"

Enemy::Enemy(Mesh *m, Shader *s) : GameObject(m, s)
{
    transform.position = glm::vec3(0.0f, 0.0f, -40.0f);
    transform.scale = glm::vec3(10.0f);
    m_hp = 1000.0f;
}

void Enemy::Init()
{
    m_hand = GetChildByName("Hand");
    m_eyebrows = GetChildByName("Eyebrows");

    GameObject::Init();
}

void Enemy::Update(float deltaTime)
{
    // transform.position.y += 20 * deltaTime;

    if (m_hand)
    {
        m_hand->transform.rotation.z += (acc++) * deltaTime * 0.05;
    }

    GameObject::Update(deltaTime);
}