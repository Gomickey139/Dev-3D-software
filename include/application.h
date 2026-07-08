#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "shader.h"
#include "mesh.h"
#include "sprite.h"
#include "gameObject.h"
#include "collisionManager.h"
#include "uiManager.h"
#include "texture.h"
#include "shotManager.h"

enum class GameState
{
    Title,
    Entrance,
    Battle,
    Clear,
    GameOver
};

class Player;
class Enemy;

class Application
{
private:
    GLFWwindow *m_window;
    int m_initialWidth, m_initialHeight; // 基準のウィンドウサイズ
    int m_windowWidth, m_windowHeight;   // 現在のウィンドウサイズ
    float m_targetAspect;                // 固定アスペクト比

    int m_viewX, m_viewY, m_viewWidth, m_viewHeight; // ビューポートの位置とサイズ

    // マネージャ

    ColliderManager m_colliderManager;
    ShotManager m_shotManager;
    std::unique_ptr<UIManager> m_uiManager; // シェーダーの読み込みが必要なためスマートポインタで管理

    // ゲームの状態管理

    float m_deltaTime;
    float m_lastFrame;

    float m_backgroundFlash = 0.0f; // クリア時の背景フラッシュ用

    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders; // シェーダーを名前とポインタのマップで管理

    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures; // テクスチャを名前とポインタのマップで管理

    std::unique_ptr<Mesh> m_bgMesh; // 背景メッシュ

    std::vector<std::unique_ptr<Mesh>> m_meshes; // メッシュの管理用ベクター

    std::vector<std::unique_ptr<GameObject>> m_gameObjects; // ゲームオブジェクトの管理用ベクター

    GameState m_gameState = GameState::Title; // ゲームの状態管理用変数

    Player *m_playerRef = nullptr; // プレイヤーのポインタ
    Enemy *m_enemyRef = nullptr;   // 敵のポインタ

    /**
     * @brief モデルを作成する
     * @param objPath OBJファイルのパス
     * @param defaultShader デフォルトのシェーダー
     * @param specialShaders パーツごとの個別シェーダー指定
     * @param name オブジェクトの名前
     * @return 作成されたGameObjectのスマートポインタ
     */
    std::unique_ptr<GameObject> CreateModel(
        const std::string &objPath,
        Shader *defaultShader,
        const std::map<std::string, Shader *> &specialShaders = {},
        std::string name = "GameObject");

    /** @brief システムに関わる入力処理を管理する
     * @note Escでの終了など
     */
    void ProcessInput(float deltaTime);
    // 各ゲームオブジェクトのInit()を呼び出す
    void Init();
    /** 各ゲームオブジェクトのUpdate()を呼び出す
     * また，状態遷移の判定もここで行う
     */
    void Update(float deltaTime);
    /** 各ゲームオブジェクトのRender()を呼び出す
     * 描画に関わること全般をここで行う
     */
    void Render();
    // ビューポートの比率を一定に保つ
    void CalculateViewport();

    // 画面フェードアウト用のメンバ　突貫実装につき直書き

    float m_clearFadeTimer = 0.0f;
    float m_gameOverFadeTimer = 0.0f;
    unsigned int m_fadeVAO, m_fadeVBO;

    void InitFadeQuad();

    // オブジェクト同士の衝突判定を行う
    void CheckCollisions();

    /**
     * @brief UI用のテクスチャを読み込み，UIManagerに登録する
     * @param name 名前
     * @param path テクスチャファイルのパス
     */
    void RegisterUITexture(const std::string &name, const std::string &path, bool isUI = true)
    {
        m_textures[name] = std::make_unique<Texture>(path, isUI);
        m_uiManager->AddTexture(name, m_textures[name]->GetID());
    }

public:
    Application();
    ~Application();

    // ライフサイクル関数
    // 画面を初期化する関数
    bool Initialize(int width, int height, const std::string &title);
    // アセットを読み込む関数
    void LoadAssets();
    // メインループを実行する関数
    void Run();
    // アプリケーションを終了する関数
    void Shutdown();
};
