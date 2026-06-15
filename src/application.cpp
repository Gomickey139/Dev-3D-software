#include "application.h"
#include "input.h"
#include "player.h"
#include "enemy.h"
#include <iostream>

Application::Application()
    : m_window(nullptr), m_windowWidth(960), m_windowHeight(540),
      m_targetAspect(960.0f / 540.0f), m_viewX(0), m_viewY(0),
      m_viewWidth(960), m_viewHeight(540), m_deltaTime(0.0f), m_lastFrame(0.0f)
{
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize(int width, int height, const std::string &title)
{
    m_windowWidth = width;
    m_windowHeight = height;
    m_targetAspect = (float)width / (float)height;

    // GLFWの初期化
    if (!glfwInit())
    {
        std::cerr << "GLFWの初期化に失敗しました" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, title.c_str(), NULL, NULL);
    if (!m_window)
    {
        std::cerr << "ウィンドウの作成に失敗しました" << std::endl;
        glfwTerminate();
        return -1;
    }

    int monitorCount;
    GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

    std::cout << "モニターの個数：" << monitorCount << std::endl;

    if (monitorCount > 1)
    {
        int xpos, ypos;
        glfwGetMonitorPos(monitors[0], &xpos, &ypos);
        glfwSetWindowPos(m_window, xpos + 100, ypos + 100);
    }

    glfwMakeContextCurrent(m_window);
    glfwSetKeyCallback(m_window, Input::KeyCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    return true;
}

std::unique_ptr<GameObject> Application::CreateModel(const std::string &objPath, Shader *defaultShader, const std::map<std::string, Shader *> &specialShaders)
{
    auto parts = ObjLoader::LoadMulti(objPath);

    auto root = std::make_unique<GameObject>(nullptr, nullptr, objPath);

    for (auto &pair : parts)
    {
        const std::string &partName = pair.first;
        std::unique_ptr<Mesh> &meshPtr = pair.second;

        Shader *shaderToUse = defaultShader;
        if (specialShaders.count(partName))
        {
            shaderToUse = specialShaders.at(partName);
        }

        auto child = std::make_unique<GameObject>(meshPtr.get(), shaderToUse, partName);

        m_meshes.push_back(std::move(meshPtr));

        root->AddChild(std::move(child));
    }

    return root;
}

void Application::LoadAssets()
{
    // シェーダーのコンパイルと確保

    m_shaders["default"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/default.frag");
    m_shaders["background"] = std::make_unique<Shader>("../assets/shaders/background/background.vert", "../assets/shaders/background/background.frag");
    m_shaders["thinking_face"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/thinking/face.frag");
    m_shaders["thinking_eyebrows"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/thinking/eyebrows.frag");
    m_shaders["shot"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/weapon/shot.frag");
    m_shaders["fire"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/jet/fire.frag");

    m_bgMesh = std::unique_ptr<Mesh>(ObjLoader::Load("../assets/models/background.obj"));

    auto player = std::make_unique<Player>(nullptr, nullptr);
    auto playerModel = CreateModel("../assets/models/jet.obj", m_shaders["default"].get(), {{"Fire", m_shaders["fire"].get()}});
    player->AddChild(std::move(playerModel));

    m_meshes.push_back(std::unique_ptr<Mesh>(ObjLoader::Load("../assets/models/shot.obj")));
    Mesh *shotMeshPtr = m_meshes.back().get();

    auto energyShot = std::make_unique<Weapon>(shotMeshPtr, m_shaders["shot"].get(), 0.15f, 100);
    player->AddWeapon(std::move(energyShot));

    m_gameObjects.push_back(std::move(player));

    auto enemy = std::make_unique<Enemy>(nullptr, nullptr);
    auto enemyModel = CreateModel("../assets/models/thinking.obj", m_shaders["thinking_face"].get(), {{"Eyebrows", m_shaders["thinking_eyebrows"].get()}});
    enemy->AddChild(std::move(enemyModel));
    m_gameObjects.push_back(std::move(enemy));
}

void Application::CalculateViewport()
{
    glfwGetWindowSize(m_window, &m_windowWidth, &m_windowHeight);

    // まずウィンドウ全体を黒（黒帯）でクリアする領域として設定
    glViewport(0, 0, m_windowWidth, m_windowHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float windowAspect = (float)m_windowWidth / (m_windowHeight == 0 ? 1 : m_windowHeight);

    m_viewWidth = m_windowWidth;
    m_viewHeight = m_windowHeight;
    m_viewX = 0;
    m_viewY = 0;

    // アスペクト比の比較によるレターボックス／ピラーボックス計算
    if (windowAspect > m_targetAspect)
    {
        // 横長すぎる場合（左右に黒帯）
        m_viewWidth = (int)(m_windowHeight * m_targetAspect);
        m_viewX = (m_windowWidth - m_viewWidth) / 2;
    }
    else
    {
        // 縦長すぎる場合（上下に黒帯）
        m_viewHeight = (int)(m_windowWidth / m_targetAspect);
        m_viewY = (m_windowHeight - m_viewHeight) / 2;
    }
}

void Application::Run()
{

    Init();

    while (!glfwWindowShouldClose(m_window))
    {
        float currentFrame = (float)glfwGetTime();
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        // 入力・イベント処理
        Input::Update();
        glfwPollEvents();
        ProcessInput(m_deltaTime);

        // 更新処理
        Update(m_deltaTime);

        // 描画処理
        Render();

        glfwSwapBuffers(m_window);
    }
}

void Application::ProcessInput(float deltaTime)
{
    if (Input::GetKeyDown(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(m_window, true);
    }
}

void Application::Init()
{
    for (auto &obj : m_gameObjects)
    {
        obj->Init();
    }
}

void Application::Update(float deltaTime)
{
    for (auto &obj : m_gameObjects)
    {
        obj->Update(deltaTime);
    }
}

void Application::Render()
{
    CalculateViewport();

    glViewport(m_viewX, m_viewY, m_viewWidth, m_viewHeight);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), m_targetAspect, 0.1f, 1000.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 10.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    float time = (float)glfwGetTime();

    if (m_bgMesh && m_shaders["background"])
    {
        glDepthMask(GL_FALSE);
        m_shaders["background"]->use();

        glm::mat4 bgView = glm::mat4(glm::mat3(view)); // 回転成分のみ抽出
        m_shaders["background"]->setMat4("projection", projection);
        m_shaders["background"]->setMat4("view", bgView);
        m_shaders["background"]->setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(200.0f)));
        m_shaders["background"]->setFloat("uTime", time);

        m_bgMesh->Draw();
        glDepthMask(GL_TRUE);
    }

    for (auto &obj : m_gameObjects)
    {
        obj->Draw(view, projection);
    }
}

void Application::Shutdown()
{
    m_gameObjects.clear();
    m_meshes.clear();
    m_bgMesh.reset();

    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}