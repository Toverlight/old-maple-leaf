#pragma once

#include <string_view>
#include <glad/glad.h>

struct TextureBinding
{
    const class Texture* texture = nullptr;
    GLuint slot = 0;
};

class Texture
{
public:
    Texture(std::string_view path);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    GLuint ID() const { return m_textureID; }

    void bind(GLuint slot = 0) const;
    void unbind() const;

private:
    GLuint m_textureID = 0;
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
};