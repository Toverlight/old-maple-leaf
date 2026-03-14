#include "renderer.h"
#include <glad/glad.h>
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

void Renderer::Submit(const Mesh& mesh, const Material& material)
{
    ML_GL_SCOPE("Renderer::Submit");
    material.bind();
    mesh.draw();

    ML_GL_CHECKPOINT("Submit end");
}