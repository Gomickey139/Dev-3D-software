#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <memory>
#include <map>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh
{
private:
    unsigned int VAO, VBO;
    size_t vertexCount;

public:
    Mesh(const std::vector<Vertex> &vertices);

    void Draw() const;

    ~Mesh();
};

class ObjLoader
{
public:
    /**
     * パスのobjファイルからメッシュを読む
     * これは複数オブジェクトの区別をしない
     */
    static Mesh *Load(const char *path);
    /**
     * obj内のオブジェクトを分けて読むメソッド
     */
    static std::map<std::string, std::unique_ptr<Mesh>> LoadMulti(const std::string &path);
};