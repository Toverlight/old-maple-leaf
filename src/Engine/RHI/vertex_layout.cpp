#include "vertex_layout.h"

bool VertexLayout::addFloat(GLuint index, GLuint count, GLboolean normalized)
{
#ifdef DEBUG
    if (hasAttribute(index) || (index >= GL_MAX_VERTEX_ATTRIBS) || count == 0) {
        return false;
    }
#endif
    VertexAttribute attr;
    attr.index = index;
    attr.count = count;
    attr.type = GL_FLOAT;
    attr.normalized = normalized;
    attr.offset = static_cast<std::uint32_t>(m_stride);
    m_attributes.push_back(attr);
    m_stride += count * sizeof(float);
    return true;
}

bool VertexLayout::addFloat(GLuint index, GLuint count, std::uint32_t offset, GLsizei stride, GLboolean normalized)
{
#ifdef DEBUG
    if (hasAttribute(index) || (index >= GL_MAX_VERTEX_ATTRIBS) || count == 0) {
        return false;
    }
    if (stride <= 0) {
        return false;
    }
#endif

    if (m_stride == 0) {
        m_stride = stride;
    }

#ifdef DEBUG
    if (m_stride != stride) {
        return false;
    }
    // Basic sanity: attribute range must fit into stride.
    const std::uint64_t end = static_cast<std::uint64_t>(offset) + static_cast<std::uint64_t>(count) * sizeof(float);
    if (end > static_cast<std::uint64_t>(stride)) {
        return false;
    }
#endif

    VertexAttribute attr;
    attr.index = index;
    attr.count = count;
    attr.type = GL_FLOAT;
    attr.normalized = normalized;
    attr.offset = offset;
    m_attributes.push_back(attr);
    return true;
}

bool VertexLayout::hasAttribute(GLuint index) const
{
    for (const auto& attr : m_attributes) {
        if (attr.index == index) {
            return true;
        }
    }
    return false;
}