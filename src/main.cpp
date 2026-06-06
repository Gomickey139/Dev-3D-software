#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "gameObject.h"
#include "input.h"
#include "player.h"

int main()
{
    // GLFWの初期化
    if (!glfwInit())
    {
        std::cerr << "GLFWの初期化に失敗しました" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(910, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cerr << "ウィンドウの作成に失敗しました" << std::endl;
        glfwTerminate();
        return -1;
    }

    int monitorCount;
    GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

    if (monitorCount > 1)
    {
        int xpos, ypos;
        glfwGetMonitorPos(monitors[0], &xpos, &ypos);
        glfwSetWindowPos(window, xpos + 100, ypos + 100);
    }

    glfwMakeContextCurrent(window);

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    glfwSetKeyCallback(window, Input::KeyCallback);

    Shader myShader("../assets/shaders/default.vert", "../assets/shaders/default.frag");
    Shader bgShader("../assets/shaders/background/background.vert", "../assets/shaders/background/background.frag");

    Mesh *myModel = ObjLoader::Load("../assets/models/suzanne.obj");
    Mesh *bgModel = ObjLoader::Load("../assets/models/background.obj");
    Mesh *playerModel = ObjLoader::Load("../assets/models/jet.obj");
    Mesh *enemyModel = ObjLoader::Load("../assets/models/thinking.obj");

    Player myPlayer(playerModel, &myShader);

    GameObject bg(bgModel, &bgShader);

    std::vector<GameObject> sceneObjects; // 画面に出すオブジェクトのリスト

    // 1体目のスザンヌ
    GameObject obj1(enemyModel, &myShader);
    obj1.transform.position = glm::vec3(-2.5f, 0.0f, -80.0f);
    obj1.transform.scale = glm::vec3(20.0f);
    sceneObjects.push_back(obj1);

    // 2体目のスザンヌ（左側・縮小）
    GameObject obj2(enemyModel, &myShader);
    obj2.transform.position = glm::vec3(-2.5f, 0.0f, -2.0f);
    obj2.transform.scale = glm::vec3(0.8f);
    sceneObjects.push_back(obj2);

    // 3体目のスザンヌ（右奥）
    GameObject obj3(enemyModel, &myShader);
    obj3.transform.position = glm::vec3(2.5f, 0.0f, -2.0f);
    sceneObjects.push_back(obj3);

    if (myModel == nullptr)
    {
        return -1; // 読み込み失敗時は終了
    }

    glEnable(GL_DEPTH_TEST);

    float deltaTime = 0.0f; // 今のフレームと前のフレームの時間差
    float lastFrame = 0.0f; // 前のフレームの時刻

    // メインループ ======================================================================
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        Input::Update();
        glfwPollEvents();
        // イベント処理
        if (Input::GetKeyDown(GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);

        float aspect = (float)width / (height == 0 ? 1 : height);
        // glm::perspective(視野角(ラジアン), アスペクト比, 最も近い描画距離, 最も遠い描画距離)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 10.0f), // カメラの位置
            glm::vec3(0.0f, 0.0f, 0.0f),  // 見つめる目標点
            glm::vec3(0.0f, 1.0f, 0.0f)   // 空間の「上」の方向
        );

        float time = glfwGetTime();

        glDepthMask(GL_FALSE);

        bgShader.use();

        // カメラの「移動成分」だけを削り、「回転成分」だけを残すテクニック
        glm::mat4 bgView = glm::mat4(glm::mat3(view));

        bgShader.setMat4("projection", projection);
        bgShader.setMat4("view", bgView);
        // 背景の箱を巨大化して配置
        bgShader.setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(100.0f)));

        bgShader.setFloat("uTime", time);

        bgModel->Draw(); // 巨大な箱を描画

        glDepthMask(GL_TRUE);

        myShader.use();

        myPlayer.Update(deltaTime);

        myPlayer.Draw(view, projection);

        sceneObjects[0].transform.rotation.y = time;        // 1体目をY軸回転
        sceneObjects[1].transform.rotation.x = time * 2.0f; // 2体目をX軸回転

        // すべてのオブジェクトを描画
        for (auto &obj : sceneObjects)
        {
            obj.Draw(view, projection);
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}