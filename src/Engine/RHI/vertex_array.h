#pragma once

#include <glad/glad.h>

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;
    void addAttribute(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) const;
private:
    GLuint m_VAO = 0;
};