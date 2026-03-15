#pragma once

#include <vector>
#include <glad/glad.h>

struct VertexAttribute
{
    GLuint index = 0;
    GLuint count = 0;
    GLenum type = GL_FLOAT;
    GLboolean normalized = GL_FALSE;
    std::uint32_t offset = 0;
};

class VertexLayout
{
public:
    bool addFloat(GLuint index, GLuint count, GLboolean normalized = GL_FALSE);
    bool addFloat(GLuint index, GLuint count, std::uint32_t offset, GLsizei stride, GLboolean normalized = GL_FALSE);

    bool hasAttribute(GLuint index) const;
    const std::vector<VertexAttribute>& getAttributes() const { return m_attributes; }
    GLsizei getStride() const { return m_stride; }
private:
    std::vector<VertexAttribute> m_attributes;
    GLsizei m_stride = 0;
};

