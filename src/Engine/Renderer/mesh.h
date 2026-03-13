#pragma once

#include <cstddef>
#include "Engine/RHI/vertex_array.h"
#include "Engine/RHI/vertex_buffer.h"

class Mesh
{
public:
    Mesh(const void* vertexData, size_t vertexDataSize, int vertexCount, const VertexLayout& layout);
    void draw() const;

private:
    VertexArray m_VAO;
    VertexBuffer m_VBO;
    int m_vertexCount = 0;
};
    