#include "application.h"
#include "input.h"
#include "player.h"
#include "enemy.h"
#include <iostream>

Application::Application()
    : m_window(nullptr), m_initialWidth(960), m_initialHeight(540), m_windowWidth(960), m_windowHeight(540),
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
    m_initialWidth = width;
    m_initialHeight = height;
    m_windowWidth = width;
    m_windowHeight = height;
    m_targetAspect = (float)width / (float)height;

    // GLFWの初期化
    if (!glfwInit())
    {
        std::cerr << "GLFWの初期化に失敗しました" << std::endl;
        return false;
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
        return false;
    }

    // WSLgではプライマリモニターが判定できず、GLFWの初期位置が
    // 別モニター側になることがあるため、仮想デスクトップ左上へ明示的に配置する。
    glfwSetWindowPos(m_window, 100, 100);
    glfwShowWindow(m_window);
    glfwFocusWindow(m_window);

    glfwMakeContextCurrent(m_window);
    // WSLgの合成速度に合わせてバッファを交換する。
    // 無制限にSwapするとGPUだけを使い続け、画面転送が追いつかない場合がある。
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false; // 起動失敗
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
    m_shaders["thinking_hand"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/thinking/hand.frag");
    m_shaders["angry_face"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/angry/face.frag");
    m_shaders["angry_eyebrows"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/angry/eyebrows.frag");
    m_shaders["shot"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/weapon/shot.frag");
    m_shaders["fire"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/jet/fire.frag");
    m_shaders["fade"] = std::make_unique<Shader>("../assets/shaders/postEffect/fade.vert", "../assets/shaders/postEffect/fade.frag");
    m_shaders["ui_hp"] = std::make_unique<Shader>("../assets/shaders/ui/ui.vert", "../assets/shaders/ui/ui.frag");
    m_shaders["tears"] = std::make_unique<Shader>("../assets/shaders/default.vert", "../assets/shaders/thinking/tears.frag");

    //===================================================================

    m_uiManager = std::make_unique<UIManager>(m_shaders["ui_hp"].get(), m_initialWidth, m_initialHeight);
    // Ui用のテクスチャの読み込み
    RegisterUITexture("player_hp_frame", "../assets/textures/PlayerHP.png", true);
    RegisterUITexture("enemy_hp_frame", "../assets/textures/EnemyHP.png", true);
    RegisterUITexture("reticle", "../assets/textures/Reticle.png", true);
    RegisterUITexture("gradient", "../assets/textures/Gradient.png", true);
    RegisterUITexture("press_enter", "../assets/textures/PRESSENTER.png", true);
    RegisterUITexture("text", "../assets/textures/Text.png", true);
    RegisterUITexture("title", "../assets/textures/Title.png", true);

    InitFadeQuad();

    m_bgMesh = std::unique_ptr<Mesh>(ObjLoader::Load("../assets/models/background.obj"));

    //===================================================================
    auto player = std::make_unique<Player>(nullptr, nullptr);
    auto playerModel = CreateModel("../assets/models/jet.obj", m_shaders["default"].get(), {{"Fire", m_shaders["fire"].get()}});
    player->AddChild(std::move(playerModel));

    auto shotModel = CreateModel("../assets/models/shot.obj", m_shaders["shot"].get(), {}, "ShotModel");
    auto playerShot = std::make_unique<ShotPool>(
        "PlayerShot",
        200,
        std::move(shotModel),
        2.0f,
        0.1f);
    ShotPool &playerShotPool = m_shotManager.AddPool(std::move(playerShot));
    auto playerWeapon = std::make_unique<Weapon>(playerShotPool, 0.15f, 70.0f);
    player->AddWeapon(std::move(playerWeapon));
    m_playerRef = player.get();
    m_gameObjects.push_back(std::move(player));
    //===================================================================

    auto enemy = std::make_unique<Enemy>(nullptr, nullptr, m_playerRef);
    auto enemyModel = CreateModel("../assets/models/thinking.obj", m_shaders["thinking_face"].get(), {{"Eyebrows", m_shaders["thinking_eyebrows"].get()}, {"Hand", m_shaders["thinking_hand"].get()}}, "EnemyModel");
    enemy->AddChild(std::move(enemyModel));
    m_enemyRef = enemy.get();

    auto angryModel = CreateModel("../assets/models/angry.obj", m_shaders["angry_face"].get(), {{"Eyebrows", m_shaders["angry_eyebrows"].get()}}, "AngryModel");
    auto enemyShot = std::make_unique<ShotPool>(
        "EnemyShot",
        500,                   // 最大弾数
        std::move(angryModel), // 弾のモデル
        5.0f,                  // 弾の寿命
        0.5f,                  // 弾の衝突判定用の半径
        ShotTeam::Enemy);
    ShotPool &enemyShotPool = m_shotManager.AddPool(std::move(enemyShot));
    auto SpreadShotWeapon = std::make_unique<Weapon_SpreadShot>(enemyShotPool, 0.1f, 30.0f);
    auto StraightShotWeapon = std::make_unique<Weapon>(enemyShotPool, 0.1f, 30.0f);

    auto tearsModel = CreateModel("../assets/models/tears.obj", m_shaders["tears"].get(), {}, "TearsModel");
    auto tearsShot = std::make_unique<ShotPool>(
        "TearsShot",
        100,                   // 最大弾数
        std::move(tearsModel), // 弾のモデル
        5.0f,                  // 弾の寿命
        1.0f,                  // 弾の衝突判定用の半径
        ShotTeam::Enemy);
    ShotPool &tearsShotPool = m_shotManager.AddPool(std::move(tearsShot));
    auto DropTearsShotWeapon = std::make_unique<Weapon>(tearsShotPool, 0.4f, 5.0f, 1.0f);

    enemy->AddWeapon(std::move(SpreadShotWeapon));
    enemy->AddWeapon(std::move(StraightShotWeapon));
    enemy->AddWeapon(std::move(DropTearsShotWeapon));
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
    if (m_gameState != GameState::Battle)
    {
        return;
    }

    m_colliderManager.CheckCollisions(m_gameObjects);

    m_colliderManager.CheckSOCollisions(m_shotManager.GetPools(), m_gameObjects);
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
    if (m_gameState == GameState::Title)
    {
        if (!Input::GetKeyDown(GLFW_KEY_ENTER))
        {
            return;
        }

        // タイトルから既存の登場演出へ移り、演出完了後にBattleへ遷移する。
        m_gameState = GameState::Entrance;
        m_playerRef->SetState(PlayerState::Wait);
    }

    for (auto &obj : m_gameObjects)
    {
        obj->Update(deltaTime);
    }

    m_shotManager.Update(deltaTime);

    switch (m_gameState)
    {
    case GameState::Title:
        break;

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
            // 敵が死んだら、クリア演出フェーズに移行
            m_gameState = GameState::Clear;

            // プレイヤーの操作を再びロックする
            m_playerRef->SetState(PlayerState::Wait);
        }
        else if (m_playerRef->GetState() == PlayerState::Dead)
        {
            // プレイヤーが死んだら、ゲームオーバー演出フェーズに移行
            m_gameState = GameState::GameOver;
        }
        break;

    case GameState::Clear:
    {
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

    case GameState::GameOver:
        m_gameOverFadeTimer += deltaTime;
        break;
    }

    if (m_uiManager)
    {
        m_uiManager->Update(deltaTime, m_playerRef, m_enemyRef);
    }
}

void Application::Render()
{
    CalculateViewport();

    glViewport(m_viewX, m_viewY, m_viewWidth, m_viewHeight);

    const float damageStrength = m_playerRef->GetDamageEffectStrength();

    const float time = static_cast<float>(glfwGetTime());

    glm::vec3 shakeOffset(std::sin(time * 83.0f), std::cos(time * 67.0f), 0.0f);

    shakeOffset *= 0.5f * damageStrength;

    const glm::vec3 baseCameraPosition(0.0f, 0.0f, 10.0f);
    const glm::vec3 baseCameraTarget(0.0f);

    glm::mat4 view = glm::lookAt(
        baseCameraPosition + shakeOffset,
        baseCameraTarget + shakeOffset,
        glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), m_targetAspect, 0.1f, 1000.0f);

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

    if (m_gameState == GameState::Title)
    {
        if (m_uiManager)
        {
            m_uiManager->DrawTitle();
        }
        return;
    }

    for (auto &obj : m_gameObjects)
    {
        obj->Draw(view, projection);
    }

    m_shotManager.Draw(view, projection);

    // ダメージを受けたときの赤いフラッシュエフェクト
    if (damageStrength > 0.0f)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        m_shaders["fade"]->use();
        m_shaders["fade"]->setFloat("uAlpha", damageStrength * 0.5f);
        m_shaders["fade"]->setVec3("uColor", glm::vec3(1.0f, 0.0f, 0.0f));

        glBindVertexArray(m_fadeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    // UI描画
    if (m_uiManager)
    {
        m_uiManager->Draw(m_playerRef, m_enemyRef);
    }

    if (m_gameState == GameState::Clear || m_gameState == GameState::GameOver)
    {
        // 半透明（アルファブレンド）を有効化
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 深度テストを無効化（奥にあっても絶対に最前面へ描画するため）
        glDisable(GL_DEPTH_TEST);

        if (m_gameState == GameState::Clear)
        {
            float alpha = glm::clamp(m_clearFadeTimer / 5.0f, 0.0f, 1.0f);

            m_shaders["fade"]->use();
            m_shaders["fade"]->setFloat("uAlpha", alpha);
            m_shaders["fade"]->setVec3("uColor", glm::vec3(1.0f));
        }
        else if (m_gameState == GameState::GameOver)
        {
            float alpha = glm::clamp(m_gameOverFadeTimer / 3.0f, 0.0f, 1.0f);

            m_shaders["fade"]->use();
            m_shaders["fade"]->setFloat("uAlpha", alpha);
            m_shaders["fade"]->setVec3("uColor", glm::vec3(0.0f));
        }

        // 四角形を描画
        glBindVertexArray(m_fadeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 設定を元に戻す
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
}

void Application::Shutdown()
{
    m_uiManager.reset();

    m_gameObjects.clear();
    m_shotManager.Clear();
    m_meshes.clear();
    m_textures.clear();
    m_bgMesh.reset();

    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}
