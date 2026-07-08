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

    glm::mat4 m_uiProjection; // UI用の正射影行列

    std::unordered_map<std::string, unsigned int> m_textureIDs;

    // UI描画関数群

    void DrawPressEnter();
    void DrawPlayerHP(Player *player);
    void DrawEnemyHP(Enemy *enemy);
    void DrawText();
    // void DrawReticle();

public:
    /** @brief UIManagerのコンストラクタ
     * @param uiShader UI用のシェーダー
     * @param initialWidth 初期ウィンドウ幅
     * @param initialHeight 初期ウィンドウ高さ
     */
    UIManager(Shader *uiShader, int initialWidth, int initialHeight);

    void Update(float deltaTime, Player *player, Enemy *enemy);

    void Draw(Player *player, Enemy *enemy);
    void DrawTitle();

    void AddTexture(const std::string &name, unsigned int id)
    {
        m_textureIDs[name] = id;
    }
};