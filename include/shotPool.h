#pragma once

#include <vector>
#include "mesh.h"
#include "shader.h"
#include "gameObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Shot
{
    glm::vec3 position;
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 velocity;
    glm::vec3 angularVelocity = glm::vec3(0.0f);
    float lifeTime;        // 残りの寿命
    bool isActive = false; // 弾が有効かどうか
};

struct ShotSpawnParams
{
    glm::vec3 position;
    glm::vec3 scale = glm::vec3(0.5f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 velocity;
    glm::vec3 angularVelocity = glm::vec3(0.0f);
};

enum class ShotTeam
{
    Player,
    Enemy
};

class ShotPool
{
protected:
    std::vector<Shot> m_shots;

    std::unique_ptr<GameObject> m_model;

    float m_shotLifeTime;
    float m_collisionRadius;

    std::string m_name;

    // virtual void UpdateMovement(float deltaTime) = 0;

    ShotTeam m_team = ShotTeam::Player;

public:
    /**
     * @brief 弾のプールを作成する
     * @param name プールの名前
     * @param maxBullets プールする弾の最大数
     * @param model 弾のモデル
     * @param lifeTime 弾の寿命（秒）
     * @param collisionRadius 弾の衝突判定用の半径
     * @param team 弾の所属チーム（プレイヤー or 敵）
     */
    ShotPool(std::string name, int maxBullets, std::unique_ptr<GameObject> model, float lifeTime, float collisionRadius, ShotTeam team = ShotTeam::Player);

    virtual ~ShotPool() = default;

    std::string GetName() { return m_name; }

    /**
     * @brief 弾を発射する
     * @param params 発射パラメータ
     */
    void Spawn(const ShotSpawnParams &params);

    void Update(float deltaTime);

    void Draw(const glm::mat4 &view, const glm::mat4 &projection);

    std::vector<Shot> &GetShots() { return m_shots; }

    float GetCollisionRadius() const { return m_collisionRadius; }

    void Collision(std::string name, Shot &s);
};
