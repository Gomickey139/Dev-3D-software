#include "shotPool.h"

ShotPool::ShotPool(int maxShots, Mesh *mesh, Shader *shader, float fireRate) : m_sharedMesh(mesh), m_sharedShader(shader), m_fireRate(fireRate)
{
    m_shots.resize(maxShots);
}

void ShotPool::UpdateAndFire(float deltaTime, bool isFireButtonPressed, std::array<glm::vec3, 2> spawnPos, glm::vec3 direction)
{
    m_fireTimer -= deltaTime;
    if (isFireButtonPressed && m_fireTimer <= 0.0f)
    {
        Fire(spawnPos[0], direction);
        Fire(spawnPos[1], direction);
        m_fireTimer = m_fireRate;
    }

    for (auto &s : m_shots)
    {
        if (!s.isActive)
        {
            continue;
        }
        s.position += s.velocity * deltaTime;
        s.lifeTime -= deltaTime;

        if (s.lifeTime <= 0.0f)
        {
            s.isActive = false;
        }
    }
}

void ShotPool::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    if (!m_sharedMesh || !m_sharedShader)
    {
        return;
    }

    m_sharedShader->use();
    m_sharedShader->setMat4("projection", projection);
    m_sharedShader->setMat4("view", view);

    for (const auto &s : m_shots)
    {
        if (!s.isActive)
        {
            continue;
        }

        glm::mat4 model = glm::translate(glm::mat4(1.0f), s.position);
        model = glm::scale(model, glm::vec3(0.5f));

        m_sharedShader->setMat4("model", model);
        m_sharedMesh->Draw();
    }
}

void ShotPool::Fire(glm::vec3 pos, glm::vec3 dir)
{
    for (auto &s : m_shots)
    {
        if (!s.isActive)
        {
            s.position = pos;
            s.velocity = dir * 70.0f;
            s.lifeTime = 2.0f;
            s.isActive = true;
            return;
        }
    }
}