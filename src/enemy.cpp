#include "enemy.h"
#include "player.h"

#include <glm/gtc/noise.hpp>
#include <iostream>

Enemy::Enemy(Mesh *m, Shader *s, Player *playerRef) : GameObject(m, s, "Enemy"), m_playerRef(playerRef)
{
    transform.position = glm::vec3(0.0f, m_startPosY, -40.0f);
    transform.scale = glm::vec3(10.0f);

    transform.rotation.y = glm::radians(m_startRotY);

    m_basePosition = glm::vec3(0.0f, 0.0f, -40.0f);
    m_hp = 10000.0f;

    collider = std::make_unique<Collider>(10.0f);
}

void Enemy::Init()
{
    m_hand = GetChildByName("Hand");
    m_eyebrows = GetChildByName("Eyebrows");
    m_face = GetChildByName("Face");
    if (!m_hand || !m_eyebrows || !m_face)
    {
        std::cerr << "Error: Enemy model is missing required child objects (Hand, Eyebrows, Face)." << std::endl;
    }

    m_baseScale = transform.scale;

    GameObject::Init();

    m_hand->transform.position = glm::vec3(-0.20817f, -0.68922f, 0.93224f);
    m_handBasePosition = m_hand->transform.position;
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
        transform.rotation.y = glm::mix(glm::radians(m_startRotY), 0.0f, v);

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

    switch (m_battle.m_currentAttackPattern)
    {
    case AttackPattern::Wait:
        break;

    case AttackPattern::StraightShot:
        UpdateStraightShot();
        break;
    case AttackPattern::SpreadShot:
        UpdateSpreadShot();
        break;
    case AttackPattern::DropTears:
        UpdateDropTears();
        break;
    case AttackPattern::PhaseChange:
        UpdatePhaseChange();
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
        glm::vec3 initialize = glm::vec3(0.0f);
        m_hand->transform.position = m_handBasePosition;
        m_eyebrows->transform.position = initialize;
        m_hand->transform.rotation = initialize;
        m_eyebrows->transform.rotation = initialize;
    }
}

void Enemy::ChangeAttackPattern(AttackPattern nextPattern)
{
    m_battle.m_currentAttackPattern = nextPattern;

    if (m_battle.m_currentPhase == 0 && m_hp <= 8000.0f)
    {
        m_battle.m_currentAttackPattern = AttackPattern::PhaseChange;
    }

    m_battle.remainingTime = 1.5f;
    m_battle.utilityTimer = 0.0f;
    m_battle.utilityCount = 0;

    switch (m_battle.m_currentAttackPattern)
    {
    case AttackPattern::Wait:
        break;
    case AttackPattern::StraightShot:
        break;
    case AttackPattern::SpreadShot:
        m_battle.utilityCount = 10;
        break;
    case AttackPattern::DropTears:
        m_battle.utilityCount = 0;
        break;
    case AttackPattern::PhaseChange:
        std::cout << "Phase Change!" << std::endl;
        m_battle.m_currentPhase++;
        break;
    }
}

void Enemy::UpdateSpreadShot()
{
    if (m_weapons.empty())
    {
        return;
    }

    m_battle.utilityTimer += m_deltaTime;

    float t = glm::clamp(m_battle.utilityTimer / 0.5f, 0.0f, 1.0f);

    if (m_battle.utilityCount <= 0)
    {
        float t = glm::clamp(m_battle.utilityTimer / 1.0f, 0.0f, 1.0f);
        float angle = glm::radians(glm::smoothstep(0.4f, 1.0f, t) * 720.0f);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

        m_hand->transform.position = glm::vec3(rotationMatrix * glm::vec4(m_handBasePosition, 1.0f));

        m_hand->transform.rotation.z = angle;
        if (t == 1.0f)
        {
            switch (m_battle.m_currentPhase)
            {
            case 0:
            {
                ChangeAttackPattern(AttackPattern::StraightShot);
                break;
            }
            case 1:
            {
                float rand = RandomUtil::Float();
                if (rand < 0.5f)
                {
                    ChangeAttackPattern(AttackPattern::StraightShot);
                }
                else
                {
                    ChangeAttackPattern(AttackPattern::DropTears);
                }
                break;
            }
            }
        }
        return;
    }

    if (t == 1.0f)
    {

        glm::vec3 localPos = glm::vec3(0.0f, 0.0f, 1.2f);
        glm::vec3 worldPos = glm::vec3(GetWorldMatrix() * glm::vec4(localPos, 1.0f));

        glm::vec3 cameraPosition(0.0f, 0.0f, 10.0f);
        glm::vec3 dir = glm::normalize(cameraPosition - worldPos);

        m_weapons[0]->Fire(worldPos, dir, 7.0f, 20);

        m_battle.utilityTimer = 0.0f;
        m_battle.utilityCount--;
    }
}

void Enemy::UpdateStraightShot()
{
    m_battle.utilityTimer += m_deltaTime;
    if (m_weapons.empty())
    {
        return;
    }

    switch (m_battle.utilityCount)
    {

    case 0:
    {
        float v = glm::smoothstep(0.4f, 1.0f, glm::clamp((m_battle.utilityTimer) / 1.0f, 0.0f, 1.0f));

        glm::vec3 targetPosition = m_playerRef->GetWorldPosition();
        glm::vec3 handPosition = m_hand->GetWorldPosition();
        glm::vec3 dir = glm::normalize(targetPosition - handPosition);

        glm::mat4 inverseParentMatrix = glm::inverse(GetWorldMatrix());
        glm::vec3 localDir = glm::normalize(glm::vec3(inverseParentMatrix * glm::vec4(dir, 0.0f)));

        float lemgth = glm::length(glm::vec2(localDir.x, localDir.z));
        glm::vec3 angles(0.0f);
        angles.x = -glm::atan(localDir.y, lemgth);
        angles.y = glm::atan(localDir.x, localDir.z);
        angles.z = 0.0f;
        m_hand->transform.rotation = {glm::mix(0.0f, angles.z, v), glm::mix(0.0f, angles.y + glm::radians(-100.0f), v), glm::mix(0.0f, -angles.x + glm::radians(-20.0f), v)};
        m_hand->transform.position = glm::vec3(m_handBasePosition.x - glm::mix(0.0f, 0.8f, v), m_handBasePosition.y + glm::mix(0.0f, 0.2f, v), m_handBasePosition.z);
        glm::vec3 shotPos = glm::vec3(m_hand->GetWorldMatrix() * glm::vec4(0.5f, 0.3f, 0.0f, 1.0f));
        glm::vec3 shotDir = glm::normalize(targetPosition - shotPos);

        if (v == 1.0f)
        {
            m_weapons[1]->Fire({shotPos}, {shotDir}, true);
        }

        if (m_battle.utilityTimer > 4.0f)
        {
            m_battle.utilityCount++;
            m_battle.utilityTimer = 0.0f;
        }
        break;
    }
    case 1:
    {
        float v = glm::smoothstep(0.4f, 1.0f, glm::clamp((m_battle.utilityTimer) / 1.0f, 0.0f, 1.0f));
        glm::vec3 targetPosition = m_playerRef->GetWorldPosition();
        glm::vec3 handPosition = m_hand->GetWorldPosition();
        glm::vec3 dir = glm::normalize(targetPosition - handPosition);

        glm::mat4 inverseParentMatrix = glm::inverse(GetWorldMatrix());
        glm::vec3 localDir = glm::normalize(glm::vec3(inverseParentMatrix * glm::vec4(dir, 0.0f)));

        float lemgth = glm::length(glm::vec2(localDir.x, localDir.z));
        glm::vec3 angles(0.0f);
        angles.x = -glm::atan(localDir.y, lemgth);
        angles.y = glm::atan(localDir.x, localDir.z);
        angles.z = 0.0f;
        m_hand->transform.position = glm::vec3(m_handBasePosition.x - glm::mix(0.8f, 0.0f, v), m_handBasePosition.y + glm::mix(0.2f, 0.0f, v), m_handBasePosition.z);
        m_hand->transform.rotation = {glm::mix(angles.z, 0.0f, v), glm::mix(angles.y + glm::radians(-100.0f), 0.0f, v), glm::mix(-angles.x + glm::radians(-20.0f), 0.0f, v)};

        if (v == 1.0f)
        {
            m_battle.utilityCount++;
            m_battle.utilityTimer = 0.0f;
        }
        break;
    }
    case 2:
    {
        float t = glm::clamp(m_battle.utilityTimer / 1.0f, 0.0f, 1.0f);
        float angle = glm::radians(glm::smoothstep(0.4f, 1.0f, t) * 720.0f);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

        m_hand->transform.position = glm::vec3(rotationMatrix * glm::vec4(m_handBasePosition, 1.0f));

        m_hand->transform.rotation.z = angle;
        if (t >= 1.0f)
        {
            switch (m_battle.m_currentPhase)
            {
            case 0:
            {
                ChangeAttackPattern(AttackPattern::SpreadShot);
                break;
            }
            case 1:
            {
                float rand = RandomUtil::Float();
                if (rand < 0.5f)
                {
                    ChangeAttackPattern(AttackPattern::SpreadShot);
                }
                else
                {
                    ChangeAttackPattern(AttackPattern::DropTears);
                }
                break;
            }
            }
        }
        break;
    }
    }
}

void Enemy::UpdateDropTears()
{
    m_battle.utilityTimer += m_deltaTime;
    if (m_weapons.empty())
    {
        return;
    }

    float posX = RandomUtil::Float(-5.8f, 5.8f);
    glm::vec3 spawnPos = glm::vec3(posX, 5.0f, 0.0f);
    glm::vec3 dir = glm::vec3(0.0f, -1.0f, 0.0f);

    m_weapons[2]->Fire({spawnPos}, {dir});

    if (m_battle.utilityTimer >= 9.0f)
    {
        float t = glm::clamp((m_battle.utilityTimer - 9.0f) / 1.0f, 0.0f, 1.0f);
        float angle = glm::radians(glm::smoothstep(0.4f, 1.0f, t) * 720.0f);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

        m_hand->transform.position = glm::vec3(rotationMatrix * glm::vec4(m_handBasePosition, 1.0f));

        m_hand->transform.rotation.z = angle;
    }

    if (m_battle.utilityTimer >= 10.0f)
    {
        if (m_battle.m_currentPhase == 1)
        {
            float rand = RandomUtil::Float();
            if (rand < 0.5f)
            {
                ChangeAttackPattern(AttackPattern::StraightShot);
            }
            else
            {
                ChangeAttackPattern(AttackPattern::SpreadShot);
            }
        }
    }
}

void Enemy::UpdatePhaseChange()
{
    m_battle.utilityTimer += m_deltaTime;
    float t = glm::clamp(m_battle.utilityTimer / 2.0f, 0.0f, 1.0f);
    float v = t * t * t;

    m_face->transform.rotation.z = glm::mix(0.0f, glm::radians(720.0f), v);
    m_eyebrows->transform.rotation.z = glm::mix(0.0f, glm::radians(720.0f), v);

    if (v == 1.0f)
    {
        ChangeAttackPattern(AttackPattern::DropTears);
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

    const glm::vec3 cameraPosition(0.0f, 0.0f, 10.0f);
    glm::vec3 dir = glm::normalize(cameraPosition - transform.position);

    glm::vec3 angles(0.0f);

    angles.x = -glm::asin(dir.y);

    angles.y = glm::atan(dir.x, dir.z);

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
        Damage(12.0f);
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
