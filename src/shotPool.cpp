#include "shotPool.h"

ShotPool::ShotPool(std::string name, int maxShots, std::unique_ptr<GameObject> model, float lifeTime, float collisionRadius, glm::vec3 scale, ShotTeam team)
    : m_name(name), m_model(std::move(model)), m_shotLifeTime(lifeTime), m_collisionRadius(collisionRadius), m_team(team)
{
    m_shots.resize(maxShots);
}
void ShotPool::Spawn(const ShotSpawnParams &params)
{
    for (auto &s : m_shots)
    {
        if (!s.isActive)
        {
            s.position = params.position;
            s.velocity = params.velocity;
            s.scale = params.scale;
            s.rotation = params.rotation;
            s.angularVelocity = params.angularVelocity;
            s.lifeTime = m_shotLifeTime;
            s.isActive = true;
            return;
        }
    }
}

void ShotPool::Update(float deltaTime)
{
    for (auto &s : m_shots)
    {
        if (!s.isActive)
        {
            continue;
        }

        s.position += s.velocity * deltaTime;
        s.rotation += s.angularVelocity * deltaTime;

        s.lifeTime -= deltaTime;

        if (s.lifeTime <= 0.0f)
        {
            s.isActive = false;
        }
    }
}

void ShotPool::Draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    if (!m_model)
    {
        return;
    }

    for (const auto &s : m_shots)
    {
        if (!s.isActive)
        {
            continue;
        }

        m_model->transform.position = s.position;
        m_model->transform.rotation = s.rotation;
        m_model->transform.scale = s.scale;

        m_model->Draw(view, projection);
    }
}

void ShotPool::Collision(std::string name, Shot &s)
{
    if (name == "Enemy" && m_team == ShotTeam::Player)
    {
        s.isActive = false;
    }
}