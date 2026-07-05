#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <iostream>

#include "shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // 見つからないときに例外を投げる
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;

        // ファイルの内容をストリームに読み込む
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        // 文字列に変換
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cerr << "エラー: シェーダーファイルの読み込みに失敗しました" << std::endl;
    }

    // std::stringからconst char*に変換
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;

    // GPU上に「空のシェーダーオブジェクト（箱）」を作成し、そのIDを返す
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // シェーダーオブジェクトにソースコードをセット
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glShaderSource(fragment, 1, &fShaderCode, NULL);

    // それをコンパイル
    glCompileShader(vertex);
    glCompileShader(fragment);

    // コンパイルエラーがないかチェック
    checkCompileErrors(vertex, "VERTEX");
    checkCompileErrors(fragment, "FRAGMENT");

    ID = glCreateProgram(); // GPU上にコンパイル済みのシェーダーを格納する「空のプログラムオブジェクト」を作成し、そのIDを返す

    // シェーダーオブジェクトをプログラムオブジェクトに「取り付ける」
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    // シェーダーをリンクして、プログラムオブジェクトを完成させる
    glLinkProgram(ID);

    // リンクエラーがないかチェック
    checkCompileErrors(ID, "PROGRAM");

    std::cout << "シェーダー読み込み成功(" << vertexPath << ", " << fragmentPath << ")" << std::endl;

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(ID); // このシェーダーを使う
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); // floatをシェーダーに渡す関数
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); // intをシェーダーに渡す関数
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); // boolをシェーダーに渡す関数
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z); // vec3をシェーダーに渡す関数
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat)); // 4x4行列をシェーダーに渡す関数
}

// コンパイル＆リンクのエラーチェックを自動で行う関数
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "コンパイルエラー (" << type << "):\n"
                      << infoLog << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "リンクエラー:\n"
                      << infoLog << std::endl;
        }
    }
}