#include "texture.h"
#include "stb_image.h"
#include <iostream>

Texture::Texture(const std::string &path, bool isUI)
{
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // UI画像用の設定　タイリングを無効に
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 画像の拡大縮小時の補間方法を設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(!isUI);

    unsigned char *data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    if (data)
    {
        GLenum format = (m_channels == 4) ? GL_RGBA : GL_RGB;

        // テクスチャデータをOpenGLに転送
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "テクスチャ読み込み成功(" << path << ")" << std::endl;
    }
    else
    {
        std::cerr << "テクスチャ読み込み失敗(" << path << ")" << std::endl;
    }
    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
}