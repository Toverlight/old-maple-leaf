#pragma once

#include <glad/glad.h>

class IndexBuffer
{
public:
    IndexBuffer();
    ~IndexBuffer();

    void bind() const;
    void unbind() const;
    void setData(GLsizeiptr size, const void* data, GLenum usage) const;
private:
    GLuint m_EBO = 0;
};