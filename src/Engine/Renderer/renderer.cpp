#include "renderer.h"
#include <glad/glad.h>
#include "material.h"
#include "mesh.h"

void Renderer::Clear()
{
    glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Submit(const Mesh& mesh, const Material& material)
{
    material.bind();
    mesh.draw();
}