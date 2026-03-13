#include "index_buffer.h"

IndexBuffer::IndexBuffer() {
    glGenBuffers(1, &m_EBO);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_EBO);
}

IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept : m_EBO(other.m_EBO) {
    other.m_EBO = 0;
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
}

void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setData(GLsizeiptr size, const void* data, GLenum usage) const {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}