#include "application.h"
#include "input.h"
#include "player.h"
#include "enemy.h"
#include "straightShotPool.h"
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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1; // 起動失敗
    }

    glfwSetKeyCallback(m_window, Input::KeyCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    return true;
}

std::unique_ptr<GameObject> Application::CreateModel(const std::string &objPath, Shader *defaultShader, const std::map<std::string, Shader *> &specialShaders, std::string name)
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
    // m_shaders["angry_face"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/angry/face.frag");
    // m_shaders["angry_face"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/angry/eyebrows.frag");
    m_shaders["shot"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/weapon/shot.frag");
    m_shaders["fire"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/jet/fire.frag");
    m_shaders["fade"] = std::make_unique<Shader>("../assets/shaders/postEffect/fade.vert", "../assets/shaders/postEffect/fade.frag");

    InitFadeQuad();

    m_bgMesh = std::unique_ptr<Mesh>(ObjLoader::Load("../assets/models/background.obj"));

    auto player = std::make_unique<Player>(nullptr, nullptr);
    auto playerModel = CreateModel("../assets/models/jet.obj", m_shaders["default"].get(), {{"Fire", m_shaders["fire"].get()}});
    player->AddChild(std::move(playerModel));

    m_meshes.push_back(std::unique_ptr<Mesh>(ObjLoader::Load("../assets/models/shot.obj")));
    Mesh *shotMeshPtr = m_meshes.back().get();

    auto playerShot = std::make_unique<StraightShotPool>("PlayerShot", 100, shotMeshPtr, m_shaders["shot"].get(), 0.15f, 70.0f, 2.0f, 0.1f);
    m_shotPools.push_back(playerShot.get());
    auto playerWeapon = std::make_unique<Weapon>(std::move(playerShot));
    player->AddWeapon(std::move(playerWeapon));
    m_playerRef = player.get();
    m_gameObjects.push_back(std::move(player));

    auto enemy = std::make_unique<Enemy>(nullptr, nullptr);
    auto enemyModel = CreateModel("../assets/models/thinking.obj", m_shaders["thinking_face"].get(), {{"Eyebrows", m_shaders["thinking_eyebrows"].get()}});
    enemy->AddChild(std::move(enemyModel));
    m_enemyRef = enemy.get();

    m_meshes.push_back(std::unique_ptr<Mesh>(ObjLoader::Load("../assets/models/angry.obj")));
    // Mesh *angryMeshPtr = m_meshes.back().get();
    // auto enemyShot = std::make_unique<StraightShotPool>("EnemyShot", 100,angryMeshPtr, m_shaders[""])
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

void Application::InitFadeQuad()
{
    // 画面全体（-1.0 ～ 1.0）を覆う、ただの大きな四角形の頂点
    float quadVertices[] = {
        -1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f,

        -1.0f, 1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f};

    glGenVertexArrays(1, &m_fadeVAO);
    glGenBuffers(1, &m_fadeVBO);

    glBindVertexArray(m_fadeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_fadeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    // x, y の2つの成分だけを送る
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

    glBindVertexArray(0);
}

void Application::CheckCollisions()
{
    m_colliderManager.CheckCollisions(m_gameObjects);

    m_colliderManager.CheckSOCollisions(m_shotPools, m_gameObjects);
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

        // 衝突判定
        CheckCollisions();

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

    switch (m_gameState)
    {
    case GameState::Entrance:
        // プレイヤーを強制的に「待機状態」にして操作不能にする
        m_playerRef->SetState(PlayerState::Wait);

        // 敵が戦闘フェーズに移行したかチェック
        if (m_enemyRef->GetState() == EnemyState::Battle)
        {
            // 敵が降りきったら、ゲーム全体を戦闘モードに移行！
            m_gameState = GameState::Battle;

            // プレイヤーの操作ロックを解除（Normalに戻す）
            m_playerRef->SetState(PlayerState::Normal);
        }
        break;

    case GameState::Battle:
        // 戦闘中... 敵が死んだかチェック
        if (m_enemyRef->GetState() == EnemyState::Death)
        {
            // 敵が死んだら、クリア演出フェーズに移行！
            m_gameState = GameState::Clear;

            // プレイヤーの操作を再びロックする（操作不能にしてクリアポーズ等へ）
            m_playerRef->SetState(PlayerState::Wait);
        }
        break;

    case GameState::Clear:
        m_clearFadeTimer += deltaTime;

        m_backgroundFlash += m_deltaTime / 5.0f;
        float v = glm::clamp(m_backgroundFlash, 0.0f, 1.0f);
        if (m_bgMesh && m_shaders["background"])
        {
            m_shaders["background"]->use();
            m_shaders["background"]->setFloat("uEndFlash", v);
        }
        break;
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

    if (m_gameState == GameState::Clear)
    {
        // 1. 半透明（アルファブレンド）を有効化
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 2. 深度テストを無効化（奥にあっても絶対に最前面へ描画するため）
        glDisable(GL_DEPTH_TEST);

        // 3. タイマーから透明度を計算（例：2.0秒かけて 0.0 → 1.0 にする）
        float alpha = glm::clamp(m_clearFadeTimer / 5.0f, 0.0f, 1.0f);

        m_shaders["fade"]->use();
        m_shaders["fade"]->setFloat("uAlpha", alpha);

        // 四角形を描画！
        glBindVertexArray(m_fadeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 4. 設定を元に戻す
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
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