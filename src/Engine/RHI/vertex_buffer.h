#pragma once

#include <glad/glad.h>

class VertexBuffer
{
public:
    VertexBuffer();
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void bind() const;
    void unbind() const;
    void setData(GLsizeiptr size, const void* data, GLenum usage) const;
private:
    GLuint m_VBO = 0;
};