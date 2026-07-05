#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <unordered_map>
#include "sprite.h"
#include "player.h"
#include "enemy.h"

class UIManager
{
private:
    std::unique_ptr<Sprite> m_uiSprite;
    Shader *m_uiShader;

    float m_playerDisplayHp = 100.0f;
    float m_enemyDisplayHp = 10000.0f;

    glm::mat4 m_uiProjection;

    std::unordered_map<std::string, unsigned int> m_textureIDs;

    void DrawPlayerHP(Player *player);
    void DrawEnemyHP(Enemy *enemy);
    // void DrawReticle();

public:
    UIManager(Shader *uiShader, int initialWidth, int initialHeight);

    void Update(float deltaTime, Player *player, Enemy *enemy);

    void Draw(Player *player, Enemy *enemy);

    void AddTexture(const std::string &name, unsigned int id)
    {
        m_textureIDs[name] = id;
    }
};