#include "sprite.h"

Sprite::Sprite()
{
    float vertices[] = {
        // 座標(x,y)   // UV(u,v)
        0.0f, 1.0f, 0.0f, 1.0f, // 左下
        0.0f, 0.0f, 0.0f, 0.0f, // 左上
        1.0f, 0.0f, 1.0f, 0.0f, // 右上

        0.0f, 1.0f, 0.0f, 1.0f, // 左下
        1.0f, 0.0f, 1.0f, 0.0f, // 右上
        1.0f, 1.0f, 1.0f, 1.0f  // 右下
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindVertexArray(0);
}

Sprite::~Sprite()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Sprite::Draw(Shader *shader, glm::vec2 position, glm::vec2 size, glm::vec3 color, float alpha, glm::mat4 projection, unsigned int textureID, bool useTexture)
{
    shader->use();
    shader->setMat4("projection", projection);
    shader->setVec3("uColor", color);
    shader->setFloat("uAlpha", alpha);

    shader->setBool("uUseTexture", useTexture);
    if (useTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        shader->setInt("uTexture", 0);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    shader->setMat4("model", model);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}