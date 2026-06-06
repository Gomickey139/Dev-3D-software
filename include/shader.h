#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    unsigned int ID;

    Shader(const char *vertexPath, const char *fragmentPath);

    void use();

    void setFloat(const std::string &name, float value) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    // コンパイル＆リンクのエラーチェックを自動で行う関数
    void checkCompileErrors(unsigned int shader, std::string type);
};