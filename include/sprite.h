#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

class Sprite
{
private:
    unsigned int m_VAO, m_VBO;

public:
    Sprite();
    ~Sprite();

    /**  描画関数
     * @param textureID
     * @param useTexture は画像を使わない場合は省略可能
     */
    void Draw(Shader *shader, glm::vec2 position, glm::vec2 size, glm::vec3 color, float alpha, glm::mat4 projection, unsigned int textureID = 0, bool useTexture = false);
};