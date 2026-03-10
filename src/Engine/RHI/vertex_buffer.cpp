#include "vertex_buffer.h"

VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &m_VBO);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_VBO);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setData(GLsizeiptr size, const void* data, GLenum usage) const {
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}