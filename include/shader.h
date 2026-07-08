#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    unsigned int ID;

    /**
     * @brief シェーダーを作成する
     * @param vertexPath 頂点シェーダーのファイルパス
     * @param fragmentPath フラグメントシェーダーのファイルパス
     */
    Shader(const char *vertexPath, const char *fragmentPath);

    // シェーダーを使用する
    void use();

    // uniform変数を設定するための関数群

    void setFloat(const std::string &name, float value) const;

    void setInt(const std::string &name, int value) const;

    void setBool(const std::string &name, bool value) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    /**
     * @brief シェーダーのコンパイルエラーをチェックする
     * @param shader シェーダーのID
     * @param type シェーダーの種類（"VERTEX", "FRAGMENT")
     */
    void checkCompileErrors(unsigned int shader, std::string type);
};