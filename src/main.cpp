#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
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
#include "Engine/Core/parser.h"
#include "Engine/Core/frame_pacer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    const AppConfig config = LoadConfig("MapleLeaf.ini");

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, config.windowConfig.resizable ? GLFW_TRUE : GLFW_FALSE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    int windowWidth = config.windowConfig.width;
    int windowHeight = config.windowConfig.height;

    GLFWmonitor* monitor = nullptr;
    if (config.windowConfig.fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            if (const GLFWvidmode* mode = glfwGetVideoMode(monitor)) {
                windowWidth = mode->width;
                windowHeight = mode->height;
            }
        }
    }

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, config.windowConfig.title.c_str(), monitor, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(config.frameConfig.swapInterval);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
#ifdef DEBUG
    GLDebug::SetupDebugOutput();
    ML_GL_CHECKPOINT("After GL init");
#endif

    glViewport(0, 0, windowWidth, windowHeight);
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

    SubMeshDesc subMeshes[] = {
        {.materialSlot = 0, .firstIndex = 0, .indexCount = 3},
        {.materialSlot = 1, .firstIndex = 3, .indexCount = 3},
    };

    VertexLayout layout = VertexPU::Layout();
    MeshDesc meshDesc;
    meshDesc.vertexData = std::as_bytes(std::span(vertices));
    meshDesc.layout = &layout;
    meshDesc.indices32 = indices;
    meshDesc.subMeshes = subMeshes;
    Mesh mesh(meshDesc);

    std::string vertexShaderSource = ReadTextFileUtf8("res/engine/shaders/basic.vert").data;

    std::string fragmentShaderSource = ReadTextFileUtf8("res/engine/shaders/basic.frag").data;

    Texture textureAris("res/engine/images/maid_aris.png");
    Texture textureArona("res/engine/images/arona.png");
    Shader shader(vertexShaderSource, fragmentShaderSource);
    std::array<Material, 2> materials = { Material(&shader), Material(&shader) };
    materials[0].setTexture("uTexture", textureAris, 0);
    materials[1].setTexture("uTexture", textureArona, 0);

    FramePacer framePacer;
    if (config.frameConfig.swapInterval == 0 && config.frameConfig.maxFps > 0) {
        framePacer.configure(config.frameConfig.maxFps, config.frameConfig.spinWaitUs);
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        processInput(window);

        Renderer::Clear();
        Renderer::Submit(mesh, materials);

        glfwSwapBuffers(window);
        framePacer.pace();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}