#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <span>
#include "Engine/Core/shader.h"
#include "Engine/Core/gl_debug.h"
#include "Engine/RHI/vertex_array.h"
#include "Engine/RHI/vertex_layout.h"
#include "Engine/Renderer/renderer.h"
#include "Engine/Renderer/mesh.h"
#include "Engine/Renderer/material.h"
#include "Engine/Renderer/vertex_types.h"
#include "Engine/Core/file.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "MapleLeaf", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
#ifdef DEBUG
    GLDebug::SetupDebugOutput();
    ML_GL_CHECKPOINT("After GL init");
#endif

    glViewport(0, 0, 1920, 1080);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // square
    VertexPU vertices[] = {
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f},
        { 0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
        { 0.5f,  0.5f, 0.0f, 1.0f, 1.0f},
        {-0.5f,  0.5f, 0.0f, 0.0f, 1.0f}
    };

    std::uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    VertexLayout layout = VertexPU::Layout();
    MeshDesc meshDesc;
    meshDesc.vertexData = std::as_bytes(std::span(vertices));
    meshDesc.layout = &layout;
    meshDesc.indices32 = indices;
    Mesh mesh(meshDesc);

    std::string vertexShaderSource = ReadTextFileUtf8("res/engine/shaders/basic.vert").data;

    std::string fragmentShaderSource = ReadTextFileUtf8("res/engine/shaders/basic.frag").data;

    Texture texture("res/engine/images/maid_aris.png");
    Shader shader(vertexShaderSource, fragmentShaderSource);
    Material material(shader);
    material.setTexture("uTexture", texture, 0);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        processInput(window);

        Renderer::Clear();
        Renderer::Submit(mesh, material);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}