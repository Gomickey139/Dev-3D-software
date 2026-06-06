#pragma once

#include "gameObject.h"
#include "input.h"

class Player : public GameObject
{
private:
    glm::vec2 velocity{0.0f, 0.0f};
    float acceleration = 120.0f;
    float limitSpeed = 15.0f;
    float minX = -5.3f, maxX = 5.3f;
    float minY = -3.5f, maxY = 3.5f;
    bool isRolling = false;
    float rollAngle = 0.0f;
    float rollTimer = 0.0f;

public:
    // コンストラクタ（MeshとShaderを受け取って親に渡す）
    Player(Mesh *m, Shader *s);

    void MoveEvent(float deltaTime);

    void InputEvent(float deltaTime);

    void limitVelocity();

    void Update(float deltaTime);
};