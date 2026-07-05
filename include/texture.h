#pragma once
#include <glad/glad.h>
#include <string>

class Texture
{
private:
    unsigned int m_textureID;
    int m_width, m_height, m_channels;

public:
    Texture(const std::string &path, bool isUI);
    ~Texture();

    unsigned int GetID() const { return m_textureID; }
};