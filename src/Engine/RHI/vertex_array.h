#pragma once

#include <glad/glad.h>

class VertexLayout;

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void bind() const;
    void unbind() const;
    void addAttribute(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) const;
    void setLayout(const VertexLayout& layout) const;
private:
    GLuint m_VAO = 0;
};