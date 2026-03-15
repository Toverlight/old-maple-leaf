#include "mesh.h"
#include <glad/glad.h>
#include <algorithm>
#include <cassert>
#include "Engine/Core/gl_debug.h"
#include "Engine/RHI/vertex_layout.h"

Mesh::Mesh(const MeshDesc& desc)
    : m_vertexCount(0)
{
    ML_GL_SCOPE("Mesh::Mesh");

    m_primitive = desc.primitive;

    const bool has16 = !desc.indices16.empty();
    const bool has32 = !desc.indices32.empty();
#ifdef DEBUG
    assert(desc.layout && "MeshDesc.layout must not be null");
    assert(!desc.vertexData.empty() && "Vertex data must be provided");
    assert(!(has16 && has32) && "Provide at most one of indices16/indices32");
#endif

    if (!desc.layout || desc.vertexData.empty()) {
        m_vertexCount = 0;
        m_indexCount = 0;
        return;
    }

    const GLsizei strideBytes = desc.layout->getStride();
    if (strideBytes <= 0) {
#ifdef DEBUG
        assert(false && "VertexLayout stride must be > 0");
#endif
        m_vertexCount = 0;
        m_indexCount = 0;
        return;
    }

    const size_t totalBytes = desc.vertexData.size_bytes();
    if ((totalBytes % static_cast<size_t>(strideBytes)) != 0) {
#ifdef DEBUG
        assert(false && "vertexData size_bytes must be a multiple of layout stride");
#endif
        m_vertexCount = 0;
        m_indexCount = 0;
        return;
    }

    const int derivedVertexCount = static_cast<int>(totalBytes / static_cast<size_t>(strideBytes));
    if (derivedVertexCount <= 0) {
#ifdef DEBUG
        assert(false && "Derived vertexCount must be > 0");
#endif
        m_vertexCount = 0;
        m_indexCount = 0;
        return;
    }

#ifdef DEBUG
    if (desc.vertexCount > 0) {
        assert(desc.vertexCount == derivedVertexCount && "vertexCount does not match vertexData/layout stride");
    }
#endif
    m_vertexCount = (desc.vertexCount > 0) ? desc.vertexCount : derivedVertexCount;

    if (has16) {
        m_indexType = GL_UNSIGNED_SHORT;
        m_indexCount = static_cast<int>(desc.indices16.size());
    } else if (has32) {
        m_indexType = GL_UNSIGNED_INT;
        m_indexCount = static_cast<int>(desc.indices32.size());
    } else {
        m_indexCount = 0;
    }

#ifdef DEBUG
    if (m_indexCount > 0) {
        if (has16) {
            const auto maxIt = std::max_element(desc.indices16.begin(), desc.indices16.end());
            const std::uint32_t maxIndex = (maxIt != desc.indices16.end()) ? *maxIt : 0u;
            assert(maxIndex < static_cast<std::uint32_t>(m_vertexCount) && "Index out of range (maxIndex >= vertexCount)");
        } else {
            const auto maxIt = std::max_element(desc.indices32.begin(), desc.indices32.end());
            const std::uint32_t maxIndex = (maxIt != desc.indices32.end()) ? *maxIt : 0u;
            assert(maxIndex < static_cast<std::uint32_t>(m_vertexCount) && "Index out of range (maxIndex >= vertexCount)");
        }
    }
#endif

    m_VAO.bind();
    m_VBO.bind();
    m_VBO.setData(static_cast<GLsizeiptr>(desc.vertexData.size_bytes()), desc.vertexData.data(), GL_STATIC_DRAW);
    m_VAO.setLayout(*desc.layout);

    if (m_indexCount > 0)
    {
        m_EBO.bind();
        if (has16) {
            m_EBO.setData(static_cast<GLsizeiptr>(desc.indices16.size_bytes()), desc.indices16.data(), GL_STATIC_DRAW);
        } else {
            m_EBO.setData(static_cast<GLsizeiptr>(desc.indices32.size_bytes()), desc.indices32.data(), GL_STATIC_DRAW);
        }
        ML_GL_CHECKPOINT("IndexBuffer::setData");
    }

    m_VBO.unbind();
    m_VAO.unbind();

    ML_GL_CHECKPOINT("Mesh setup");
}

void Mesh::draw() const
{
    ML_GL_SCOPE("Mesh::draw");

    if (m_vertexCount <= 0) {
        return;
    }

    m_VAO.bind();
    if (m_indexCount > 0) {
        glDrawElements(m_primitive, m_indexCount, m_indexType, nullptr);
        ML_GL_CHECKPOINT("glDrawElements");
    } else {
        glDrawArrays(m_primitive, 0, static_cast<GLsizei>(m_vertexCount));
        ML_GL_CHECKPOINT("glDrawArrays");
    }
    m_VAO.unbind();
}
