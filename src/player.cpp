#include "player.h"
#include <iostream>

Player::Player(Mesh *m, Shader *s) : GameObject(m, s, "Player")
{
    transform.position = glm::vec3(0.0f, -2.0f, 0.0f);
    transform.scale = glm::vec3(0.1f);
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    m_hp = 100.0f;

    collider = std::make_unique<Collider>(0.5f);
}

void Player::Init()
{
    m_fire = GetChildByName("Fire");
}

void Player::MoveEvent()
{
    glm::vec2 targetVel = {0.0f, 0.0f};
    if (Input::GetKey(GLFW_KEY_W))
        targetVel.y += 1.0f;

    if (Input::GetKey(GLFW_KEY_A))
        targetVel.x -= 1.0f;

    if (Input::GetKey(GLFW_KEY_S))
        targetVel.y -= 1.0f;

    if (Input::GetKey(GLFW_KEY_D))
        targetVel.x += 1.0f;

    if (glm::length(targetVel) > 0.0f)
        targetVel = glm::normalize(targetVel) * limitSpeed;

    glm::vec2 diff = targetVel - velocity;
    float dist = glm::length(diff);

    float maxAccel = acceleration * m_deltaTime;

    if (dist <= maxAccel)
    {
        velocity = targetVel;
    }
    else
    {
        velocity += glm::normalize(diff) * maxAccel;
    }
}

void Player::UpdateWeapons(bool canFire)
{

    if (canFire && Input::GetKey(GLFW_KEY_P))
    {

        glm::vec3 localOffset1 = glm::vec3(-3.0f, -0.5f, 2.0f);
        glm::vec3 localOffset2 = glm::vec3(3.0f, -0.5f, 2.0f);

        glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);

        glm::mat4 worldMatrix = GetWorldMatrix();

        glm::vec3 spawnPos1 = glm::vec3(worldMatrix * glm::vec4(localOffset1, 1.0f));
        glm::vec3 spawnPos2 = glm::vec3(worldMatrix * glm::vec4(localOffset2, 1.0f));

        glm::vec3 forwardDir = glm::normalize(dir + glm::vec3(velocity, 0.0f) * 0.02f);

        // 連射と更新を一括で処理
        m_weapons[0]->Fire({spawnPos1, spawnPos2}, {forwardDir, forwardDir});
    }

    for (auto &w : m_weapons)
    {
        w->Update(m_deltaTime);
    }
}

void Player::InputEvent()
{
    MoveEvent();
}

void Player::limitVelocity()
{
    float speed = glm::length(velocity);

    if (speed > limitSpeed && speed > 0.0f)
    {
        velocity = glm::normalize(velocity) * limitSpeed;
    }
}

void Player::UpdateWait()
{
    m_rotateRisetTimer += m_deltaTime;

    velocity = glm::vec2(0.0f);

    float value = glm::clamp(m_rotateRisetTimer / 0.2f, 0.0f, 1.0f);

    transform.rotation = glm::mix(m_currentRotate, glm::vec3(0.0f), value);
}

void Player::UpdateNormal()
{
    InputEvent();
    limitVelocity();

    transform.rotation.z = -velocity.x * 0.03f;
    transform.rotation.x = velocity.y * 0.03f;

    transform.position.x += velocity.x * m_deltaTime;
    transform.position.y += velocity.y * m_deltaTime;

    if (Input::GetKeyDown(GLFW_KEY_SPACE))
    {
        ChangeState(PlayerState::Rolling);
    }
}

void Player::UpdateRolling()
{
    InputEvent();
    limitVelocity();

    float rollDuration = 1.2f;
    m_rollTimer += m_deltaTime;

    if (m_invincibleTimer < 1.0f)
    {
        m_invincibleTimer += m_deltaTime;
    }

    // 1. ローリングの進行度（イージング）を計算
    float t = glm::clamp(m_rollTimer / rollDuration, 0.0f, 1.0f);
    float invT = 1.0f - t;
    float easeT = 1.0f - (invT * invT * invT);

    m_rollAngle = glm::radians(720.0f) * easeT;

    float targetBaseRotZ = -velocity.x * 0.03f;
    float blend = 0.0f;

    float blendStartTime = 1.0f;
    if (m_rollTimer > blendStartTime)
    {
        blend = glm::clamp((m_rollTimer - blendStartTime) / (rollDuration - blendStartTime), 0.0f, 1.0f);
    }

    // Z軸は「ローリングの回転」＋「終わり際だけ復活する横移動の傾き」
    transform.rotation.z = m_rollAngle + (targetBaseRotZ * blend);

    // X軸（上下）の傾きはローリングと干渉しないので、そのまま生かしておく
    transform.rotation.x = velocity.y * 0.03f;

    // 3. 移動処理
    transform.position.x += velocity.x * m_deltaTime;
    transform.position.y += velocity.y * m_deltaTime;

    // 4. 終了判定
    if (m_rollTimer >= rollDuration)
    {
        if (m_invincibleTimer < 1.0f)
        {
            ChangeState(PlayerState::AfterTakingDamage);
        }
        ChangeState(PlayerState::Normal);
    }
}

void Player::UpdateDead()
{
}

void Player::UpdateAfterTakingDamage()
{

    InputEvent();
    limitVelocity();

    transform.rotation.z = -velocity.x * 0.03f;
    transform.rotation.x = velocity.y * 0.03f;

    transform.position.x += velocity.x * m_deltaTime;
    transform.position.y += velocity.y * m_deltaTime;

    m_invincibleTimer += m_deltaTime;

    float value = glm::clamp(m_invincibleTimer / 1.0f, 0.0f, 1.0f);

    if (value >= 1.0f)
    {
        ChangeState(PlayerState::Normal);
    }
    else if (Input::GetKeyDown(GLFW_KEY_SPACE))
    {
        ChangeState(PlayerState::Rolling);
    }
}

void Player::ChangeState(PlayerState nextState)
{
    m_currentState = nextState;

    switch (m_currentState)
    {
    case PlayerState::Wait:
        m_rotateRisetTimer = 0.0f;
        m_currentRotate = transform.rotation;
        break;

    case PlayerState::Rolling:
        m_rollTimer = 0.0f;
        m_rollAngle = 0.0f;
        acceleration = 360.0f;
        break;

    case PlayerState::Normal:
        m_rollAngle = 0.0f;
        acceleration = 120.0f;
        break;

    case PlayerState::Dead:
        break;

    case PlayerState::AfterTakingDamage:
        m_invincibleTimer = 0.0f;
        break;
    }
}

void Player::Update(float deltaTime)
{
    m_deltaTime = deltaTime;
    m_damageEffectTimer = std::max(0.0f, m_damageEffectTimer - deltaTime);

    switch (m_currentState)
    {
    case PlayerState::Wait:
        UpdateWait();
        break;
    case PlayerState::Normal:
        UpdateNormal();
        break;
    case PlayerState::Rolling:
        UpdateRolling();
        break;
    case PlayerState::Dead:
        UpdateDead();
        break;
    case PlayerState::AfterTakingDamage:
        UpdateAfterTakingDamage();
        break;
    }

    transform.position.x = glm::clamp(transform.position.x, minX, maxX);
    transform.position.y = glm::clamp(transform.position.y, minY, maxY);

    bool canFire = (m_currentState == PlayerState::Normal || m_currentState == PlayerState::Rolling);

    UpdateWeapons(canFire);

    GameObject::Update(deltaTime);
}

void Player::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    GameObject::Draw(view, projection);
}

void Player::AddWeapon(std::unique_ptr<Weapon> weapon)
{
    m_weapons.push_back(std::move(weapon));
}

void Player::Damage(float damage)
{
    m_hp -= damage;
    m_damageEffectTimer = DAMAGE_EFFECT_DURATION;
    if (m_hp <= 0.0f)
    {
        m_hp = 0.0f;
        ChangeState(PlayerState::Dead);
    }
}

void Player::Collision(std::string name)
{
    if (m_currentState == PlayerState::Dead ||
        m_currentState == PlayerState::AfterTakingDamage ||
        m_currentState == PlayerState::Rolling)
    {
        return;
    }

    bool damaged = false;

    if (name == "EnemyShot")
    {
        Damage(10.0f);
        damaged = true;
    }
    else if (name == "TearsShot")
    {
        Damage(15.0f);
        damaged = true;
    }

    if (damaged && m_currentState != PlayerState::Dead)
    {
        ChangeState(PlayerState::AfterTakingDamage);
    }
}

float Player::GetDamageEffectStrength() const
{
    return glm::clamp(m_damageEffectTimer / DAMAGE_EFFECT_DURATION, 0.0f, 1.0f);
}
