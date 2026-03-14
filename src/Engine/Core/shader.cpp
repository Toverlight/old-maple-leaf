#include "shader.h"
#include <iostream>
#include "Engine/Core/gl_debug.h"

Shader::Shader(std::string_view vertexSource, std::string_view fragmentSource) {
    ML_GL_SCOPE("Shader::Shader");
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vSrc = vertexSource.data();
    glShaderSource(vertexShader, 1, &vSrc, nullptr);
    glCompileShader(vertexShader);

    ML_GL_CHECKPOINT("Compile vertex shader");

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader Compilation Failed:\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fSrc = fragmentSource.data();
    glShaderSource(fragmentShader, 1, &fSrc, nullptr);
    glCompileShader(fragmentShader);

    ML_GL_CHECKPOINT("Compile fragment shader");

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader Compilation Failed:\n" << infoLog << std::endl;
    }

    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);

    ML_GL_LABEL(GL_PROGRAM, m_programID, "ShaderProgram");
    ML_GL_CHECKPOINT("Link shader program");

    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Failed:\n" << infoLog << std::endl;
        m_programID = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(m_programID);
}

void Shader::use() const {
    glUseProgram(m_programID);
}

GLint Shader::getUniformLocation(std::string_view name) const {
    const std::string uniformName(name);

    const auto it = m_uniformLocations.find(uniformName);
    if (it != m_uniformLocations.end()) {
        return it->second;
    }
    
    const GLint location = glGetUniformLocation(m_programID, uniformName.c_str());
    m_uniformLocations.emplace(uniformName, location);

    if (location == -1) {
        logMissingUniform(uniformName);
    }

    return location;
}

void Shader::logMissingUniform(std::string_view name) const {
#ifdef DEBUG
    const std::string uniformName(name);

    if (m_missingUniformWarnings.find(uniformName) == m_missingUniformWarnings.end()) {
        m_missingUniformWarnings.insert(uniformName);
        std::cerr << "Warning: Uniform '" << uniformName << "' not found in shader program " << m_programID << std::endl;
    }
#endif
}