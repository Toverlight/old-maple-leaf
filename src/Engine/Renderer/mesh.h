#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include "Engine/RHI/index_buffer.h"
#include "Engine/RHI/vertex_array.h"
#include "Engine/RHI/vertex_buffer.h"

class VertexLayout;

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
};

class Mesh
{
public:
    Mesh(const MeshDesc& desc);
    void draw() const;

private:
    VertexArray m_VAO;
    VertexBuffer m_VBO;
    IndexBuffer m_EBO;
    int m_vertexCount = 0;
    int m_indexCount = 0;
    GLenum m_indexType = GL_UNSIGNED_INT;
    GLenum m_primitive = GL_TRIANGLES;
};
    