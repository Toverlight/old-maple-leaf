#include "renderer.h"
#include <glad/glad.h>
#include <cassert>
#include "material.h"
#include "mesh.h"
#include "Engine/Core/gl_debug.h"

void Renderer::Clear()
{
    // ML_GL_SCOPE("Renderer::Clear");
    glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // ML_GL_CHECKPOINT("Clear");
}

void Renderer::Submit(const Mesh& mesh, std::span<const class Material> materials)
{
    ML_GL_SCOPE("Renderer::Submit");

#ifdef DEBUG
    assert(!materials.empty() && "Renderer::Submit requires at least one material");
#endif
    if (materials.empty()) {
        return;
    }

    const auto subMeshes = mesh.getSubMeshes();
    for (size_t i = 0; i < subMeshes.size(); ++i) {
        const std::uint32_t slot = subMeshes[i].materialSlot;

#ifdef DEBUG
        assert(slot < materials.size() && "SubMesh materialSlot out of range");
#endif
        const size_t safeSlot = (slot < materials.size()) ? static_cast<size_t>(slot) : 0;

        materials[safeSlot].bind();
        mesh.drawSubMesh(i);
    }

    ML_GL_CHECKPOINT("Submit end");
}