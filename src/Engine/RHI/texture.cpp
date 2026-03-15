#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include "Engine/Core/gl_debug.h"

Texture::Texture(std::string_view path)
{
    const std::string pathStr(path);

    glGenTextures(1, &m_textureID);
#ifdef DEBUG
    if (m_textureID == 0) {
        std::cerr << "Failed to generate texture for: " << path << std::endl;
    }
#endif

#ifdef DEBUG
    ML_GL_LABEL(GL_TEXTURE, m_textureID, pathStr.c_str());
#endif
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(pathStr.c_str(), &m_width, &m_height, &m_channels, 0);
    if (data) {
        GLenum format;
        GLenum internalFormat;
        switch (m_channels) {
            case 1: format = GL_RED; internalFormat = GL_R8; break;
            case 3: format = GL_RGB; internalFormat = GL_RGB8; break;
            case 4: format = GL_RGBA; internalFormat = GL_RGBA8; break;
            default:
                std::cerr << "Unsupported number of channels (" << m_channels << ") in texture: " << path << std::endl;
                stbi_image_free(data);
                glBindTexture(GL_TEXTURE_2D, 0);
                return;
        }

        // stb_image returns tightly-packed rows. For RGB (3 bytes/pixel) and R (1 byte/pixel),
        // OpenGL's default GL_UNPACK_ALIGNMENT=4 can cause row misalignment -> stripes/garbage.
        GLint prevUnpackAlignment = 4;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlignment);

        ML_GL_CHECKPOINT("Texture upload+mipmap");
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
}

Texture::Texture(Texture&& other) noexcept
    : m_textureID(other.m_textureID), m_width(other.m_width), m_height(other.m_height), m_channels(other.m_channels)
{
    other.m_textureID = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_channels = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other) {
        glDeleteTextures(1, &m_textureID);

        m_textureID = other.m_textureID;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;

        other.m_textureID = 0;
        other.m_width = 0;
        other.m_height = 0;
        other.m_channels = 0;
    }
    return *this;
}

void Texture::bind(GLuint slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}