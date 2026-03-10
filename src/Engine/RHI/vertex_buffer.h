#pragma once

#include <glad/glad.h>

class VertexBuffer
{
public:
    VertexBuffer();
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
    void setData(GLsizeiptr size, const void* data, GLenum usage) const;
private:
    GLuint m_VBO = 0;
};