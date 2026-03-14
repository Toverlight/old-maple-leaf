#include "mesh.h"
#include <glad/glad.h>
#include "Engine/Core/gl_debug.h"

Mesh::Mesh(const void* vertexData, size_t vertexDataSize, int vertexCount, const VertexLayout& layout)
    : m_vertexCount(vertexCount)
{
    ML_GL_SCOPE("Mesh::Mesh");
    m_VAO.bind();
    m_VBO.bind();
    m_VBO.setData(vertexDataSize, vertexData, GL_STATIC_DRAW);
    m_VAO.setLayout(layout);
    m_VBO.unbind();
    m_VAO.unbind();

    ML_GL_CHECKPOINT("Mesh setup");
}

void Mesh::draw() const
{
    ML_GL_SCOPE("Mesh::draw");
    m_VAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
    m_VAO.unbind();

    ML_GL_CHECKPOINT("glDrawArrays");
}
