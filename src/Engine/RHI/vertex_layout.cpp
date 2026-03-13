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

bool VertexLayout::hasAttribute(GLuint index) const
{
    for (const auto& attr : m_attributes) {
        if (attr.index == index) {
            return true;
        }
    }
    return false;
}