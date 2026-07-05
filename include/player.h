#pragma once

#include "gameObject.h"
#include "input.h"
#include "weapon.h"

enum class PlayerState
{
    Wait,
    Normal,
    Rolling,
    AfterTakingDamage,
    Dead
};

class Player : public GameObject
{
private:
    glm::vec2 velocity{0.0f, 0.0f};
    float acceleration = 120.0f;
    float limitSpeed = 15.0f;
    float minX = -5.8f, maxX = 5.8f;
    float minY = -3.5f, maxY = 3.5f;
    bool m_isRolling = false;
    float m_rollAngle = 0.0f;
    float m_rollTimer = 0.0f;
    float m_rotateRisetTimer = 0.0f;
    glm::vec3 m_currentRotate;
    float m_hp = 0;

    float m_deltaTime;

    float m_invincibleTimer = 0.0f;

    GameObject *m_fire = nullptr;

    std::vector<std::unique_ptr<Weapon>> m_weapons;

    PlayerState m_currentState = PlayerState::Normal;

    void UpdateWait();
    void UpdateNormal();
    void UpdateRolling();
    void UpdateAfterTakingDamage();
    void UpdateDead();

    void ChangeState(PlayerState nextState);

public:
    // コンストラクタ（MeshとShaderを受け取って親に渡す）
    Player(Mesh *m, Shader *s);

    void Init() override;

    void MoveEvent();

    void UpdateWeapons(bool canFire);

    void InputEvent();

    void limitVelocity();

    void Update(float deltaTime) override;

    void Draw(const glm::mat4 &view, const glm::mat4 &projection) override;

    void AddWeapon(std::unique_ptr<Weapon> weapon);

    float GetHP() const { return m_hp; }

    void SetState(PlayerState newState) { ChangeState(newState); }

    void Damage(float damage);

    void Collision(std::string name) override;

    PlayerState GetState() { return m_currentState; }
};