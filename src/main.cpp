#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Engine/Core/shader.h"
#include "Engine/RHI/vertex_array.h"
#include "Engine/RHI/vertex_layout.h"
#include "Engine/Renderer/renderer.h"
#include "Engine/Renderer/mesh.h"
#include "Engine/Renderer/material.h"

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

    glViewport(0, 0, 1920, 1080);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f
    };

    VertexLayout layout;
    layout.addFloat(0, 3, GL_FALSE); // position3
    layout.addFloat(1, 3, GL_FALSE); // color3
    layout.addFloat(2, 2, GL_FALSE); // texcoord2
    Mesh mesh(vertices, sizeof(vertices), 3, layout);

    const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec2 TexCoord;
void main() {
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
)";

    const char* fragmentShaderSource = R"(
#version 460 core
uniform sampler2D uTexture;
in vec3 ourColor;
in vec2 TexCoord;
out vec4 FragColor;
void main() {
    FragColor = texture(uTexture, TexCoord) * vec4(ourColor, 1.0);
}
)";

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