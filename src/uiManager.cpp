#include "uiManager.h"

UIManager::UIManager(Shader *uiShader, int initialWidth, int initialHeight) : m_uiShader(uiShader)
{
    m_uiSprite = std::make_unique<Sprite>();

    m_uiProjection = glm::ortho(0.0f, (float)initialWidth, (float)initialHeight, 0.0f, -1.0f, 1.0f);
}

void UIManager::Update(float deltaTime, Player *player, Enemy *enemy)
{
    if (player)
    {
        m_playerDisplayHp = glm::mix(m_playerDisplayHp, player->GetHP(), 10.0f * deltaTime);
    }

    if (enemy)
    {
        m_enemyDisplayHp = glm::mix(m_enemyDisplayHp, enemy->GetHP(), 10.0f * deltaTime);
    }
}

void UIManager::Draw(Player *player, Enemy *enemy)
{
    // OpenGLのUI描画設定
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (player)
        DrawPlayerHP(player);
    if (enemy)
        DrawEnemyHP(enemy);

    DrawText();
    // 状態を戻す
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void UIManager::DrawTitle()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    DrawPressEnter();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void UIManager::DrawPressEnter()
{
    unsigned int pressEnterTextureID = m_textureIDs["press_enter"];
    if (pressEnterTextureID != 0)
    {
        glm::vec2 pos = glm::vec2(452.8f, 453.3f);
        glm::vec2 size = glm::vec2(535.2f);
        m_uiSprite->Draw(m_uiShader, pos, size, glm::vec3(1.0f), 1.0f, m_uiProjection, pressEnterTextureID, true);
    }
}

void UIManager::DrawPlayerHP(Player *player)
{
    float actualHp = player->GetHP();
    float maxBarWidth = 262.0f;
    float barHeight = 20.0f;
    glm::vec2 barPos = glm::vec2(26.5f, 765.0f); // 画面左下

    // 背景・残像・実HPの描画

    unsigned int gradientTextureID = m_textureIDs["gradient"];

    m_uiSprite->Draw(m_uiShader, barPos, glm::vec2(maxBarWidth, barHeight), glm::vec3(0.2f), 0.8f, m_uiProjection);
    m_uiSprite->Draw(m_uiShader, barPos, glm::vec2(maxBarWidth * (m_playerDisplayHp / 100.0f), barHeight), glm::vec3(0.8f, 0.1f, 0.1f), 1.0f, m_uiProjection);
    if (gradientTextureID != 0)
    {
        m_uiSprite->Draw(m_uiShader, barPos, glm::vec2(maxBarWidth * (actualHp / 100.0f), barHeight), glm::vec3(0.1f, 0.8f, 0.2f), 1.0f, m_uiProjection, gradientTextureID, true);
    }
    else
    {
        m_uiSprite->Draw(m_uiShader, barPos, glm::vec2(maxBarWidth * (actualHp / 100.0f), barHeight), glm::vec3(0.1f, 0.8f, 0.2f), 1.0f, m_uiProjection);
    }

    unsigned int frameTextureID = m_textureIDs["player_hp_frame"];

    if (frameTextureID != 0)
    {
        glm::vec2 framePos = glm::vec2(17.9f, 734.5f);
        glm::vec2 frameSize = glm::vec2(279.2f);
        m_uiSprite->Draw(m_uiShader, framePos, frameSize, glm::vec3(1.0f), 1.0f, m_uiProjection, frameTextureID, true);
    }
}

void UIManager::DrawEnemyHP(Enemy *enemy)
{
    float actualHp = enemy->GetHP();
    float maxBarWidth = 476.0f;
    float barHeight = 20.0f;
    glm::vec2 barPos = glm::vec2(937.0f, 766.0f);

    unsigned int gradientTextureID = m_textureIDs["gradient"];

    m_uiSprite->Draw(m_uiShader, barPos, glm::vec2(maxBarWidth, barHeight), glm::vec3(0.2f), 0.8f, m_uiProjection);
    m_uiSprite->Draw(m_uiShader, barPos, glm::vec2(maxBarWidth * (m_enemyDisplayHp / 10000.0f), barHeight), glm::vec3(0.8f, 0.1f, 0.1f), 1.0f, m_uiProjection); // 残像
    if (gradientTextureID != 0)
    {
        m_uiSprite->Draw(m_uiShader, barPos, glm::vec2(maxBarWidth * (actualHp / 10000.0f), barHeight), glm::vec3(1.0f, 1.0f, 0.2f), 1.0f, m_uiProjection, gradientTextureID, true);
    }
    else
    {
        m_uiSprite->Draw(m_uiShader, barPos, glm::vec2(maxBarWidth * (actualHp / 10000.0f), barHeight), glm::vec3(1.0f, 1.0f, 0.2f), 1.0f, m_uiProjection);
    }
    unsigned int frameTextureID = m_textureIDs["enemy_hp_frame"];

    if (frameTextureID != 0)
    {
        glm::vec2 framePos = glm::vec2(927.9f, 701.7f);
        glm::vec2 frameSize = glm::vec2(494.2f);
        m_uiSprite->Draw(m_uiShader, framePos, frameSize, glm::vec3(1.0f), 1.0f, m_uiProjection, frameTextureID, true);
    }
}

void UIManager::DrawText()
{
    unsigned int textTextureID = m_textureIDs["text"];
    if (textTextureID != 0)
    {
        glm::vec2 pos = glm::vec2(5.0f, 5.0f);
        glm::vec2 size = glm::vec2(238.0f);
        m_uiSprite->Draw(m_uiShader, pos, size, glm::vec3(1.0f), 1.0f, m_uiProjection, textTextureID, true);
    }
}