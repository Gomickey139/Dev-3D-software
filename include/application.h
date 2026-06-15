#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "shader.h"
#include "mesh.h"
#include "gameObject.h"

class Application
{
private:
    GLFWwindow *m_window;
    int m_windowWidth;
    int m_windowHeight;
    float m_targetAspect;

    int m_viewX, m_viewY, m_viewWidth, m_viewHeight;

    float m_deltaTime;
    float m_lastFrame;

    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;

    std::unique_ptr<Mesh> m_bgMesh;

    std::vector<std::unique_ptr<Mesh>> m_meshes;

    std::vector<std::unique_ptr<GameObject>> m_gameObjects;

    std::unique_ptr<GameObject> CreateModel(
        const std::string &objPath,
        Shader *defaultShader,
        const std::map<std::string, Shader *> &specialShaders = {} // パーツごとの個別シェーダー指定
    );

    void ProcessInput(float deltaTime);
    void Init();
    void Update(float deltaTime);
    void Render();
    void CalculateViewport();

public:
    Application();
    ~Application();

    // ライフサイクル関数
    bool Initialize(int width, int height, const std::string &title);
    void LoadAssets();
    void Run();
    void Shutdown();
};
