#pragma once

#include "gameObject.h"

enum class EnemyState
{
    Entrance,
    Battle,
    Death
};

class Enemy : public GameObject
{
private:
    float m_hp = 0;
    float acc = 2.0f;
    float m_deltaTime;
    float m_lifeTime = 0.0f;
    glm::vec3 m_basePosition;
    glm::vec3 m_baseScale;

    float m_hitEffectTimer = 0.0f;
    const float HIT_EFFECT_DURATION = 0.15f;

    float m_deathTimer = 0.0f;

    GameObject *m_hand = nullptr;
    GameObject *m_eyebrows = nullptr;

    EnemyState m_currentState = EnemyState::Entrance;

    int m_entrancePhase = 0;
    float m_entranceTimer = 0.0f;
    float m_startPosY = 40.0f;
    float m_startRotX = -1.0f;
    float m_startRotY = 3.14f;

    void UpdateEntrance();
    void UpdateBattle();
    void UpdateDeath();

    void ChangeState(EnemyState nextState);

public:
    Enemy(Mesh *m, Shader *s);

    void Init() override;

    void Update(float deltaTime) override;

    void Draw(const glm::mat4 &view, const glm::mat4 &projection) override;

    void Drift();

    void Damage(float damage);

    void Collision(std::string name) override;

    void HitEffect();

    void ApplyHitFlash(GameObject *node, float flashAmount);
    void ApplyDeathDarkOut(GameObject *node, float darkAmount);

    EnemyState GetState() { return m_currentState; }
};