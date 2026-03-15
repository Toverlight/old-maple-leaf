#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "Engine/RHI/index_buffer.h"
#include "Engine/RHI/vertex_array.h"
#include "Engine/RHI/vertex_buffer.h"

class VertexLayout;

struct SubMeshDesc
{
    GLenum primitive = GL_TRIANGLES;

    // Material slot for higher-level systems (Model/Renderer) to map to a Material.
    std::uint32_t materialSlot = 0;

    // If indexCount > 0, this submesh is indexed.
    std::uint32_t firstIndex = 0;
    std::uint32_t indexCount = 0;
    std::int32_t baseVertex = 0;

    // If vertexCount > 0 (and indexCount == 0), this submesh is non-indexed.
    std::uint32_t firstVertex = 0;
    std::uint32_t vertexCount = 0;
};

struct MeshDesc
{
    // Optional. If 0, will be derived from vertexData.size_bytes() / layout->getStride().
    int vertexCount = 0;

    // Raw vertex bytes. Use std::as_bytes(std::span(vertices)) to fill.
    std::span<const std::byte> vertexData{};

    const VertexLayout* layout = nullptr;

    // Provide at most one of these.
    std::span<const std::uint16_t> indices16{};
    std::span<const std::uint32_t> indices32{};

    GLenum primitive = GL_TRIANGLES;

    std::span<const SubMeshDesc> subMeshes{};
};

struct SubMesh
{
    GLenum primitive = GL_TRIANGLES;

    bool indexed = false;

    std::uint32_t firstIndex = 0;
    std::uint32_t indexCount = 0;
    std::int32_t baseVertex = 0;

    std::uint32_t firstVertex = 0;
    std::uint32_t vertexCount = 0;

    std::uint32_t materialSlot = 0;
};

class Mesh
{
public:
    Mesh(const MeshDesc& desc);
    void draw() const;

    std::span<const SubMesh> getSubMeshes() const { return m_subMeshes; }
    void drawSubMesh(size_t index) const;

private:
    VertexArray m_VAO;
    VertexBuffer m_VBO;
    IndexBuffer m_EBO;
    int m_vertexCount = 0;
    int m_indexCount = 0;
    GLenum m_indexType = GL_UNSIGNED_INT;
    GLenum m_primitive = GL_TRIANGLES;

    std::vector<SubMesh> m_subMeshes;
};
    