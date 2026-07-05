#include "enemy.h"

#include <glm/gtc/noise.hpp>
#include <iostream>

Enemy::Enemy(Mesh *m, Shader *s) : GameObject(m, s, "Enemy")
{
    transform.position = glm::vec3(0.0f, m_startPosY, -40.0f);
    transform.scale = glm::vec3(10.0f);

    transform.rotation.y = m_startRotY;

    m_basePosition = glm::vec3(0.0f, 0.0f, -40.0f);
    m_hp = 10000.0f;

    collider = std::make_unique<Collider>(10.0f);
}

void Enemy::Init()
{
    m_hand = GetChildByName("Hand");
    m_eyebrows = GetChildByName("Eyebrows");

    m_baseScale = transform.scale;

    GameObject::Init();
}

void Enemy::UpdateEntrance()
{
    if (m_entrancePhase == 0)
    {
        m_entranceTimer += m_deltaTime;
        float t = glm::clamp(m_entranceTimer / 10.0f, 0.0f, 1.0f);
        float v = glm::smoothstep(0.2f, 0.7f, t);
        transform.position.y = glm::mix(m_startPosY, 0.0f, v);
        transform.rotation.x = glm::mix(m_startRotX, 0.0f, v);

        if (transform.position.y <= 0.0f)
        {
            m_entranceTimer = 0.0f;
            m_entrancePhase++;
        }
    }
    else if (m_entrancePhase == 1)
    {
        m_entranceTimer += m_deltaTime;
        float t = glm::clamp(m_entranceTimer / 4.0f, 0.0f, 1.0f);
        float v = glm::smoothstep(0.2f, 0.8f, t);
        transform.rotation.y = glm::mix(m_startRotY, 0.0f, v);

        if (transform.rotation.y <= 0.0f)
        {
            m_entranceTimer = 0.0f;
            m_entrancePhase++;
        }
    }
    else if (m_entrancePhase == 2)
    {
        ChangeState(EnemyState::Battle);
    }
}

void Enemy::UpdateBattle()
{
    Drift();

    m_battle.remainingTime -= m_deltaTime;

    switch (m_battle.m_currentAttackPattern)
    {
    case AttackPattern::Wait:
        if (m_battle.remainingTime > 3.0f)
        {
            m_battle.remainingTime = 0.0f;
            m_battle.m_currentAttackPattern = AttackPattern::SpreadShot;
        }
        break;

    case AttackPattern::StraightShot:
        /* code */
        break;
    case AttackPattern::SpreadShot:
        if (m_battle.remainingTime < 0.0f)
        {
            m_battle.remainingTime = 1.5f;
            m_battle.m_currentAttackPattern = AttackPattern::SpreadShot;
        }
        UpdateSpreadShot();
        break;
    default:
        break;
    }

    HitEffect();
}

void Enemy::UpdateDeath()
{
    HitEffect();

    float expSpeed = 0.3f * m_deltaTime;
    m_hand->transform.position.y -= expSpeed;
    m_eyebrows->transform.position.y += expSpeed;

    m_deathTimer += m_deltaTime;
    float v = glm::clamp(m_deathTimer / 4.0f, 0.0f, 1.0f);
    ApplyDeathDarkOut(this, v);
}

void Enemy::ChangeState(EnemyState nextState)
{
    m_currentState = nextState;

    if (m_currentState == EnemyState::Death)
    {
        m_hand->transform.rotation.z = 0.0f;
    }
}

void Enemy::UpdateSpreadShot()
{
    if (m_weapons.empty())
    {
        return;
    }

    m_battle.fireIntervalTimer += m_deltaTime;
    float t = glm::clamp(m_battle.fireIntervalTimer / 0.5f, 0.0f, 1.0f);
    if (t == 1.0f)
    {
        if (m_battle.utilityCount >= 5)
        {
            m_battle.m_currentAttackPattern = AttackPattern::Wait;
            m_battle.remainingTime = 1.5f;
            m_battle.utilityCount = 0;
            return;
        }
        glm::vec3 localPos = glm::vec3(0.0f, 0.0f, 1.2f);
        glm::vec3 worldPos = glm::vec3(GetWorldMatrix() * glm::vec4(localPos, 1.0f));

        glm::vec3 cameraPosition(0.0f, 0.0f, 10.0f);
        glm::vec3 dir = glm::normalize(cameraPosition - worldPos);

        m_weapons[0]->Fire(worldPos, dir, 7.0f, 20);

        m_battle.fireIntervalTimer = 0.0f;
        m_battle.utilityCount++;
    }
    else
    {
        return;
    }
}

void Enemy::Update(float deltaTime)
{
    m_deltaTime = deltaTime;

    switch (m_currentState)
    {
    case EnemyState::Entrance:
        UpdateEntrance();
        break;
    case EnemyState::Battle:
        UpdateBattle();
        break;
    case EnemyState::Death:
        UpdateDeath();
        break;
    }

    GameObject::Update(deltaTime);
    for (auto &w : m_weapons)
    {
        w->Update(m_deltaTime);
    }
}

void Enemy::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    float flashAmount = (m_hitEffectTimer > 0.0f) ? (m_hitEffectTimer / HIT_EFFECT_DURATION) : 0.0f;

    ApplyHitFlash(this, flashAmount);

    GameObject::Draw(view, projection);

    for (auto &w : m_weapons)
    {
        w->Draw(view, projection);
    }
}

void Enemy::Drift()
{
    m_lifeTime += m_deltaTime;

    float driftSpeed = 0.1f;

    float driftX = 20.0f;
    float driftY = 10.0f;

    float noiseX = glm::simplex(glm::vec2(m_lifeTime * driftSpeed, 0.0f)) * driftX;
    float noiseY = glm::simplex(glm::vec2(m_lifeTime * driftSpeed, 100.0f)) * driftY;

    float fadeTime = 2.0f;
    float weight = glm::clamp(m_lifeTime / fadeTime, 0.0f, 1.0f);

    transform.position = m_basePosition + glm::vec3(noiseX, noiseY, 0.0f) * weight;

    glm::vec3 dir = glm::normalize(-transform.position);

    glm::vec3 angles(0.0f);

    angles.x = -glm::asin(dir.y) * 0.5;

    angles.y = glm::atan(dir.x, dir.z) * 0.5;

    angles.z = 0.0f;

    transform.rotation = angles;
}

void Enemy::Damage(float damage)
{
    m_hp -= damage;

    if (m_hp < 0)
    {
        std::cout << "死" << std::endl;
        ChangeState(EnemyState::Death);
    }
}

void Enemy::Collision(std::string name)
{
    if (name == "PlayerShot" && m_currentState == EnemyState::Battle)
    {
        // std::cout << "衝突" << std::endl;
        m_hitEffectTimer = HIT_EFFECT_DURATION;
        Damage(10.0f);
    }
}

void Enemy::HitEffect()
{
    float currentScaleMultiplier = 1.0f;

    if (m_hitEffectTimer)
    {
        m_hitEffectTimer -= m_deltaTime;

        float t = glm::clamp(m_hitEffectTimer / HIT_EFFECT_DURATION, 0.0f, 1.0f);

        currentScaleMultiplier = 1.0f + std::sin(t * 3.14159f) * 0.03f;
    }

    transform.scale = m_baseScale * currentScaleMultiplier;
}

void Enemy::ApplyHitFlash(GameObject *node, float flashAmount)
{
    if (node->shader)
    {
        node->shader->use();
        node->shader->setFloat("uHitFlash", flashAmount);
    }

    for (auto &child : node->children)
    {
        ApplyHitFlash(child.get(), flashAmount);
    }
}

void Enemy::ApplyDeathDarkOut(GameObject *node, float darkAmount)
{
    if (node->shader)
    {
        node->shader->use();
        node->shader->setFloat("uDeathDark", darkAmount);
    }

    for (auto &child : node->children)
    {
        ApplyDeathDarkOut(child.get(), darkAmount);
    }
}

void Enemy::AddWeapon(std::unique_ptr<Weapon> weapon)
{
    m_weapons.push_back(std::move(weapon));
}