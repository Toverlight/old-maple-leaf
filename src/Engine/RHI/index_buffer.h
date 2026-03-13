#pragma once

#include <glad/glad.h>

class IndexBuffer
{
public:
    IndexBuffer();
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&& other) noexcept;
    IndexBuffer& operator=(IndexBuffer&& other) noexcept;

    void bind() const;
    void unbind() const;
    void setData(GLsizeiptr size, const void* data, GLenum usage) const;
private:
    GLuint m_EBO = 0;
};