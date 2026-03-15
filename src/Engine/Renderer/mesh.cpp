#include "mesh.h"
#include <glad/glad.h>
#include <algorithm>
#include <cassert>
#include "Engine/Core/gl_debug.h"
#include "Engine/RHI/vertex_layout.h"

namespace {
size_t IndexElementSize(GLenum indexType)
{
    switch (indexType) {
    case GL_UNSIGNED_SHORT: return sizeof(std::uint16_t);
    case GL_UNSIGNED_INT: return sizeof(std::uint32_t);
    default: return 0;
    }
}
}

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

    // Build submesh list (drawcall slices)
    m_subMeshes.clear();
    if (!desc.subMeshes.empty()) {
        m_subMeshes.reserve(desc.subMeshes.size());
        for (const SubMeshDesc& smd : desc.subMeshes) {
            const bool wantsIndexed = (smd.indexCount > 0);
            const bool wantsNonIndexed = (!wantsIndexed && smd.vertexCount > 0);

#ifdef DEBUG
            assert(!(wantsIndexed && wantsNonIndexed) && "SubMeshDesc cannot be both indexed and non-indexed");
            if (wantsIndexed) {
                assert(m_indexCount > 0 && "Indexed SubMeshDesc provided but mesh has no index buffer");
                assert(static_cast<std::uint64_t>(smd.firstIndex) + static_cast<std::uint64_t>(smd.indexCount)
                       <= static_cast<std::uint64_t>(m_indexCount) && "SubMeshDesc index range out of bounds");
            }
            if (wantsNonIndexed) {
                assert(static_cast<std::uint64_t>(smd.firstVertex) + static_cast<std::uint64_t>(smd.vertexCount)
                       <= static_cast<std::uint64_t>(m_vertexCount) && "SubMeshDesc vertex range out of bounds");
            }
            assert((wantsIndexed || wantsNonIndexed) && "SubMeshDesc must specify either indexCount>0 or vertexCount>0");
#endif

            // Release behavior: skip invalid submeshes.
            if (wantsIndexed) {
                if (m_indexCount <= 0) {
                    continue;
                }
                const std::uint64_t end = static_cast<std::uint64_t>(smd.firstIndex) + static_cast<std::uint64_t>(smd.indexCount);
                if (end > static_cast<std::uint64_t>(m_indexCount)) {
                    continue;
                }

                SubMesh sm;
                sm.primitive = smd.primitive;
                sm.indexed = true;
                sm.firstIndex = smd.firstIndex;
                sm.indexCount = smd.indexCount;
                sm.baseVertex = smd.baseVertex;
                sm.materialSlot = smd.materialSlot;
                m_subMeshes.push_back(sm);
            } else if (wantsNonIndexed) {
                const std::uint64_t end = static_cast<std::uint64_t>(smd.firstVertex) + static_cast<std::uint64_t>(smd.vertexCount);
                if (end > static_cast<std::uint64_t>(m_vertexCount)) {
                    continue;
                }

                SubMesh sm;
                sm.primitive = smd.primitive;
                sm.indexed = false;
                sm.firstVertex = smd.firstVertex;
                sm.vertexCount = smd.vertexCount;
                sm.materialSlot = smd.materialSlot;
                m_subMeshes.push_back(sm);
            }
        }
    }

    // If no submeshes were provided (or all were invalid), build a default one.
    if (m_subMeshes.empty()) {
        SubMesh sm;
        sm.primitive = desc.primitive;
        sm.materialSlot = 0;
        if (m_indexCount > 0) {
            sm.indexed = true;
            sm.firstIndex = 0;
            sm.indexCount = static_cast<std::uint32_t>(m_indexCount);
            sm.baseVertex = 0;
        } else {
            sm.indexed = false;
            sm.firstVertex = 0;
            sm.vertexCount = static_cast<std::uint32_t>(m_vertexCount);
        }
        m_subMeshes.push_back(sm);
    }

    ML_GL_CHECKPOINT("Mesh setup");
}

void Mesh::draw() const
{
    ML_GL_SCOPE("Mesh::draw");

    if (m_vertexCount <= 0 || m_subMeshes.empty()) {
        return;
    }

    m_VAO.bind();
    const size_t indexElemSize = IndexElementSize(m_indexType);
    for (const SubMesh& subMesh : m_subMeshes) {
        if (subMesh.indexed) {
            if (subMesh.indexCount == 0) {
                continue;
            }
            const void* indexOffset = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(subMesh.firstIndex) * indexElemSize);
            if (subMesh.baseVertex != 0) {
                glDrawElementsBaseVertex(subMesh.primitive, static_cast<GLsizei>(subMesh.indexCount), m_indexType, indexOffset,
                                         static_cast<GLint>(subMesh.baseVertex));
                ML_GL_CHECKPOINT("glDrawElementsBaseVertex");
            } else {
                glDrawElements(subMesh.primitive, static_cast<GLsizei>(subMesh.indexCount), m_indexType, indexOffset);
                ML_GL_CHECKPOINT("glDrawElements");
            }
        } else {
            if (subMesh.vertexCount == 0) {
                continue;
            }
            glDrawArrays(subMesh.primitive, static_cast<GLint>(subMesh.firstVertex), static_cast<GLsizei>(subMesh.vertexCount));
            ML_GL_CHECKPOINT("glDrawArrays");
        }
    }
    m_VAO.unbind();
}

void Mesh::drawSubMesh(size_t index) const
{
    ML_GL_SCOPE("Mesh::drawSubMesh");

    if (index >= m_subMeshes.size()) {
        return;
    }

    const SubMesh& subMesh = m_subMeshes[index];
    m_VAO.bind();
    if (subMesh.indexed) {
        if (subMesh.indexCount == 0) {
            m_VAO.unbind();
            return;
        }
        const size_t indexElemSize = IndexElementSize(m_indexType);
        const void* indexOffset = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(subMesh.firstIndex) * indexElemSize);
        if (subMesh.baseVertex != 0) {
            glDrawElementsBaseVertex(subMesh.primitive, static_cast<GLsizei>(subMesh.indexCount), m_indexType, indexOffset,
                                     static_cast<GLint>(subMesh.baseVertex));
            ML_GL_CHECKPOINT("glDrawElementsBaseVertex");
        } else {
            glDrawElements(subMesh.primitive, static_cast<GLsizei>(subMesh.indexCount), m_indexType, indexOffset);
            ML_GL_CHECKPOINT("glDrawElements");
        }
    } else {
        if (subMesh.vertexCount == 0) {
            m_VAO.unbind();
            return;
        }
        glDrawArrays(subMesh.primitive, static_cast<GLint>(subMesh.firstVertex), static_cast<GLsizei>(subMesh.vertexCount));
        ML_GL_CHECKPOINT("glDrawArrays");
    }
    m_VAO.unbind();
}
