#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <iostream>

#include "mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices)
{
    vertexCount = vertices.size();

    glGenVertexArrays(1, &VAO); // データの読み方の説明書のIDを発行
    glGenBuffers(1, &VBO);      // データを入れる箱のIDを発行

    glBindVertexArray(VAO);                                                                           // VAOをバインドして、以降の設定をこのVAOに関連付ける
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                                               // VBOをバインドして、以降の頂点データの操作をこのVBOに関連付ける
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW); // CPUからGPUに頂点データを転送する

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0); // GPUに頂点属性のレイアウトを説明する。ここでは、位置データがどのように格納されているかを指定している
    glEnableVertexAttribArray(0);                                               // 頂点属性を有効にする。ここでは、位置データの属性を有効にしている

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal)); // GPUに頂点属性のレイアウトを説明する。ここでは、法線データがどのように格納されているかを指定している
    glEnableVertexAttribArray(1);                                                                      // 頂点属性を有効にする。ここでは、法線データの属性を有効にしている

    glBindVertexArray(0); // VAOのバインドを解除して、以降の頂点属性の設定がこのVAOに影響しないようにする
}

void Mesh::Draw() const
{
    glBindVertexArray(VAO);                     // VAOをバインドして、以降の描画コマンドがこのVAOに関連付けられた頂点データを使用するようにする
    glDrawArrays(GL_TRIANGLES, 0, vertexCount); // VAOに関連付けられた頂点データを使用して、オブジェクトを描画する
    glBindVertexArray(0);                       // VAOのバインドを解除して、以降の描画コマンドがこのVAOに影響しないようにする
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO); // VAOを削除して、GPUのリソースを解放する
    glDeleteBuffers(1, &VBO);      // VBOを削除して、GPUのリソースを解放する
}

Mesh *ObjLoader::Load(const char *path)
{
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;
    std::vector<Vertex> out_vertices;

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "エラー: OBJファイルが開けません: " << path << std::endl;
        return nullptr;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v")
        {
            glm::vec3 p;
            ss >> p.x >> p.y >> p.z;
            temp_positions.push_back(p);
        }
        else if (type == "vn")
        {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            temp_normals.push_back(n);
        }
        else if (type == "f")
        {
            std::string vStr;
            std::vector<std::string> faceVertices;

            auto parseFaceVertex = [&](const std::string &vStr)
            {
                int pIdx = 0, tIdx = 0, nIdx = 0;
                if (vStr.find("//") != std::string::npos)
                {
                    sscanf(vStr.c_str(), "%d//%d", &pIdx, &nIdx);
                }
                else
                {
                    sscanf(vStr.c_str(), "%d/%d/%d", &pIdx, &tIdx, &nIdx);
                }

                Vertex vertex;
                vertex.position = temp_positions[pIdx - 1];
                vertex.normal = temp_normals[nIdx - 1];
                out_vertices.push_back(vertex);
            };

            while (ss >> vStr)
            {
                faceVertices.push_back(vStr);
            }

            for (size_t i = 1; i < faceVertices.size() - 1; ++i)
            {
                parseFaceVertex(faceVertices[0]);
                parseFaceVertex(faceVertices[i]);
                parseFaceVertex(faceVertices[i + 1]);
            }
        }
    }

    std::cout << "モデル読み込み成功(" << path << "): 頂点数 = " << out_vertices.size() << std::endl;

    return new Mesh(out_vertices);
}

std::map<std::string, std::unique_ptr<Mesh>> ObjLoader::LoadMulti(const std::string &path)
{
    std::map<std::string, std::unique_ptr<Mesh>> result;

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;

    std::vector<Vertex> currentVertices;
    std::string currentPartName = "DefaultPart";

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "エラー: OBJファイルが開けません: " << path << std::endl;
        return result;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "o" || type == "g")
        {
            if (!currentVertices.empty())
            {
                result[currentPartName] = std::make_unique<Mesh>(currentVertices);

                currentVertices.clear();
            }

            ss >> currentPartName;
        }
        else if (type == "v")
        {
            glm::vec3 p;
            ss >> p.x >> p.y >> p.z;
            temp_positions.push_back(p);
        }
        else if (type == "vn")
        {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            temp_normals.push_back(n);
        }
        else if (type == "f")
        {
            std::string vStr;
            std::vector<std::string> faceVertices;

            auto parseFaceVertex = [&](const std::string &vStr)
            {
                int pIdx = 0, tIdx = 0, nIdx = 0;
                if (vStr.find("//") != std::string::npos)
                {
                    sscanf(vStr.c_str(), "%d//%d", &pIdx, &nIdx);
                }
                else
                {
                    sscanf(vStr.c_str(), "%d/%d/%d", &pIdx, &tIdx, &nIdx);
                }

                Vertex vertex;
                vertex.position = temp_positions[pIdx - 1];
                vertex.normal = temp_normals[nIdx - 1];
                currentVertices.push_back(vertex);
            };

            while (ss >> vStr)
            {
                faceVertices.push_back(vStr);
            }

            for (size_t i = 1; i < faceVertices.size() - 1; ++i)
            {
                parseFaceVertex(faceVertices[0]);
                parseFaceVertex(faceVertices[i]);
                parseFaceVertex(faceVertices[i + 1]);
            }
        }
    }

    if (!currentVertices.empty())
    {
        result[currentPartName] = std::make_unique<Mesh>(currentVertices);
    }

    std::cout << "モデル読み込み成功(" << path << ") 複数オブジェクト" << std::endl;

    return result;
}
