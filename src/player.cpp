#include "player.h"
#include <iostream>

Player::Player(Mesh *m, Shader *s) : GameObject(m, s)
{
    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(0.1f);
    velocity.x = 0.0f;
    velocity.y = 0.0f;
}

void Player::MoveEvent(float deltaTime)
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

    float maxAccel = acceleration * deltaTime;

    if (dist <= maxAccel)
    {
        velocity = targetVel;
    }
    else
    {
        velocity += glm::normalize(diff) * maxAccel;
    }
}

void Player::InputEvent(float deltaTime)
{
    MoveEvent(deltaTime);

    if (Input::GetKeyDown(GLFW_KEY_SPACE) && !isRolling) // 無敵
    {
        isRolling = true;
        rollAngle = 0.0f;
        acceleration *= 3.0f;
        rollTimer = 0.0f;
    }
}

void Player::limitVelocity()
{
    float speed = glm::length(velocity);

    if (speed > limitSpeed && speed > 0.0f)
    {
        velocity = glm::normalize(velocity) * limitSpeed;
    }
}

void Player::Update(float deltaTime)
{
    InputEvent(deltaTime);
    limitVelocity();

    transform.rotation.z = -velocity.x * 0.03f;
    transform.rotation.x = velocity.y * 0.03f;

    float targetBaseRotZ = -velocity.x * 0.03f;
    float targetBaseRotX = velocity.y * 0.03f;

    if (isRolling)
    {

        float rollDuration = 1.2f;
        rollTimer += deltaTime;

        float t = glm::clamp(rollTimer / rollDuration, 0.0f, 1.0f);

        float invT = 1.0f - t;
        float easeT = 1.0f - (invT * invT * invT);

        float targetAngle = glm::radians(720.0f);
        rollAngle = targetAngle * easeT;

        float blendStartTime = 1.0f; // 補間を開始する時間
        float blend = 0.0f;          // 0.0(無効) 〜 1.0(本来の傾き100%) の割合

        if (rollTimer > blendStartTime)
        {
            blend = glm::clamp((rollTimer - blendStartTime) / (rollDuration - blendStartTime), 0.0f, 1.0f);
        }

        transform.rotation.z = glm::mix(0.0f, targetBaseRotZ, blend);
        transform.rotation.x = glm::mix(0.0f, targetBaseRotX, blend);

        if (rollTimer >= rollDuration)
        {
            rollAngle = 0.0f;
            isRolling = false;
            acceleration /= 3.0f;
        }
    }

    transform.rotation.z += rollAngle;

    transform.position.x += velocity.x * deltaTime;
    transform.position.y += velocity.y * deltaTime;
    transform.position.x = glm::clamp(transform.position.x, minX, maxX);
    transform.position.y = glm::clamp(transform.position.y, minY, maxY);
}