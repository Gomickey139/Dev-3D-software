#pragma once

#include "gameObject.h"
#include "weapon_spreadShot.h"

enum class EnemyState
{
    Entrance,
    Battle,
    Death
};

enum class AttackPattern
{
    Wait,
    StraightShot,
    SpreadShot
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
    const float HIT_EFFECT_DURATION = 0.1f;

    float m_deathTimer = 0.0f;

    GameObject *m_hand = nullptr;
    GameObject *m_eyebrows = nullptr;

    std::vector<std::unique_ptr<Weapon>> m_weapons;

    EnemyState m_currentState = EnemyState::Entrance;

    int m_entrancePhase = 0;
    float m_entranceTimer = 0.0f;
    float m_startPosY = 40.0f;
    float m_startRotX = -1.0f;
    float m_startRotY = 3.14f;

    struct BattleData
    {
        AttackPattern m_currentAttackPattern = AttackPattern::SpreadShot;
        float remainingTime = 1.5f;
        float fireIntervalTimer = 0.0f; // 発射間隔のタイマー
        int utilityCount = 0;
    } m_battle;

    void UpdateEntrance();
    void UpdateBattle();
    void UpdateDeath();

    void ChangeState(EnemyState nextState);

    void UpdateSpreadShot();

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

    void AddWeapon(std::unique_ptr<Weapon> weapon);

    float GetHP() const { return m_hp; }

    EnemyState GetState() { return m_currentState; }
};