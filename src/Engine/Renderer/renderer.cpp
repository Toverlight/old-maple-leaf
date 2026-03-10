#include "renderer.h"
#include <glad/glad.h>

void Renderer::Clear()
{
    glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}