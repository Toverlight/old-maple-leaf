#include "vertex_array.h"
#include "vertex_layout.h"
#include "Engine/Core/gl_debug.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_VAO);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_VAO);
}

VertexArray::VertexArray(VertexArray&& other) noexcept : m_VAO(other.m_VAO) {
    other.m_VAO = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    if (this != &other) {
        if (m_VAO != 0) {
            glDeleteVertexArrays(1, &m_VAO);
        }
        m_VAO = other.m_VAO;
        other.m_VAO = 0;
    }
    return *this;
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

void VertexArray::setLayout(const VertexLayout& layout) const {
    for (const auto& attr : layout.getAttributes()) {
        addAttribute(attr.index, attr.count, attr.type, attr.normalized, layout.getStride(), reinterpret_cast<const void*>(static_cast<std::uintptr_t>(attr.offset)));
    }

    ML_GL_CHECKPOINT("VertexArray::setLayout");
}