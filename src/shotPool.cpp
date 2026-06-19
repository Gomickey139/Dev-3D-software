#include "shotPool.h"

ShotPool::ShotPool(std::string name, int maxShots, Mesh *mesh, Shader *shader, float fireRate, float speed, float lifeTime, float collisionRadius, glm::vec3 scale)
    : m_name(name), m_sharedMesh(mesh), m_sharedShader(shader), m_fireRate(fireRate), m_shotSpeed(speed), m_shotLifeTime(lifeTime), m_collisionRadius(collisionRadius), m_shotScale(scale)
{
    m_shots.resize(maxShots);
}
void ShotPool::UpdateAndFire(float deltaTime, bool isFireButtonPressed, const std::vector<glm::vec3> spawnPos, glm::vec3 direction)
{
    m_fireTimer -= deltaTime;
    if (isFireButtonPressed && m_fireTimer <= 0.0f)
    {
        for (const auto &pos : spawnPos)
        {
            Fire(pos, direction);
        }
        m_fireTimer = m_fireRate;
    }

    UpdateMovement(deltaTime);

    for (auto &s : m_shots)
    {
        if (!s.isActive)
        {
            continue;
        }

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
        model = glm::scale(model, m_shotScale);

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
            s.velocity = dir * m_shotSpeed; // 70.0
            s.lifeTime = m_shotLifeTime;    // 2.0
            s.isActive = true;
            return;
        }
    }
}

void ShotPool::Collision(std::string name, Shot &s)
{
    if (name == "Enemy")
    {
        s.isActive = false;
    }
}