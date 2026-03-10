#include "vertex_array.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_VAO);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_VAO);
}

void VertexArray::bind() const {
    glBindVertexArray(m_VAO);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::addAttribute(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) const {
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(index);
}