#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "shader.h"
#include "mesh.h"
#include "gameObject.h"
#include "collisionManager.h"

enum class GameState
{
    Entrance,
    Battle,
    Clear,
    Death
};

class Player;
class Enemy;

class Application
{
private:
    GLFWwindow *m_window;
    int m_windowWidth;
    int m_windowHeight;
    float m_targetAspect;

    int m_viewX, m_viewY, m_viewWidth, m_viewHeight;

    ColliderManager m_colliderManager;

    float m_deltaTime;
    float m_lastFrame;

    float m_backgroundFlash = 0.0f;

    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;

    std::unique_ptr<Mesh> m_bgMesh;

    std::vector<std::unique_ptr<Mesh>> m_meshes;

    std::vector<std::unique_ptr<GameObject>> m_gameObjects;

    std::vector<ShotPool *> m_shotPools;

    GameState m_gameState = GameState::Entrance;

    Player *m_playerRef = nullptr;
    Enemy *m_enemyRef = nullptr;

    std::unique_ptr<GameObject> CreateModel(
        const std::string &objPath,
        Shader *defaultShader,
        const std::map<std::string, Shader *> &specialShaders = {}, // パーツごとの個別シェーダー指定
        std::string name = "GameObject"                             //
    );

    void ProcessInput(float deltaTime);
    void Init();
    void Update(float deltaTime);
    void Render();
    void CalculateViewport();

    float m_clearFadeTimer = 0.0f;
    unsigned int m_fadeVAO, m_fadeVBO;

    void InitFadeQuad();

    void CheckCollisions();

public:
    Application();
    ~Application();

    // ライフサイクル関数
    bool Initialize(int width, int height, const std::string &title);
    void LoadAssets();
    void Run();
    void Shutdown();
};
