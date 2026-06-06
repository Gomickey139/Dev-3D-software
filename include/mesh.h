#pragma once

#include <vector>
#include <string>

struct Vec3
{
    float x, y, z;
};

struct Vertex
{
    Vec3 position;
    Vec3 normal;
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
    static Mesh *Load(const char *path);
};