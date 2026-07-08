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
    PhaseChange,
    StraightShot,
    SpreadShot,
    DropTears
};

class Player;

class Enemy : public GameObject
{
private:
    float m_deltaTime;

    float m_hp = 0;           // HP
    float m_lifeTime = 0.0f;  // 経過時間．Driftの計算などに使用
    glm::vec3 m_basePosition; // 初期位置
    glm::vec3 m_baseScale;    // 初期スケール

    float m_hitEffectTimer = 0.0f;          // ヒットエフェクトのタイマー
    const float HIT_EFFECT_DURATION = 0.1f; // ヒットエフェクトの長さ（秒）

    float m_deathTimer = 0.0f; // 死亡演出のタイマー

    GameObject *m_hand = nullptr;
    GameObject *m_eyebrows = nullptr;
    GameObject *m_face = nullptr;

    glm::vec3 m_handBasePosition; // 手の初期位置

    std::vector<std::unique_ptr<Weapon>> m_weapons;

    EnemyState m_currentState = EnemyState::Entrance;

    int m_entrancePhase = 0;      // 入場演出の段階を管理
    float m_entranceTimer = 0.0f; // 入場演出のタイマー
    float m_startPosY = 40.0f;
    float m_startRotX = -1.0f;
    float m_startRotY = 180.0f; // 入場時の初期回転(オイラー)

    Player *m_playerRef = nullptr; // プレイヤーのポインタ

    // バトル中のデータをまとめた構造体
    struct BattleData
    {
        AttackPattern m_currentAttackPattern = AttackPattern::SpreadShot;
        int m_currentPhase = 0; // 攻撃パターンの段階を管理
        float remainingTime = 1.5f;
        float utilityTimer = 0.0f; // 汎用タイマー
        int utilityCount = 10;     // 汎用カウンター
    } m_battle;

    void UpdateEntrance();
    void UpdateBattle();
    void UpdateDeath();

    void ChangeState(EnemyState nextState);
    void ChangeAttackPattern(AttackPattern nextPattern);

    void UpdateSpreadShot();
    void UpdateStraightShot();
    void UpdateDropTears();
    void UpdatePhaseChange();

public:
    Enemy(Mesh *m, Shader *s, Player *playerRef);

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