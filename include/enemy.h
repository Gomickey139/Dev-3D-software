#pragma once

#include "gameObject.h"

class Enemy : public GameObject
{
private:
    float m_hp = 0;
    float acc = 2.0f;

    GameObject *m_hand = nullptr;
    GameObject *m_eyebrows = nullptr;

public:
    Enemy(Mesh *m, Shader *s);

    void Init() override;

    void Update(float deltaTime) override;
};