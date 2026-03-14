#pragma once

#include <glad/glad.h>

#ifdef DEBUG
#include <iostream>
#endif

namespace GLDebug {
#ifdef DEBUG

inline const char* SourceToStr(GLenum source) {
    switch (source) {
    case GL_DEBUG_SOURCE_API: return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WindowSystem";
    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "ShaderCompiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY: return "ThirdParty";
    case GL_DEBUG_SOURCE_APPLICATION: return "Application";
    case GL_DEBUG_SOURCE_OTHER: return "Other";
    default: return "Unknown";
    }
}

inline const char* TypeToStr(GLenum type) {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR: return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined";
    case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
    case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
    case GL_DEBUG_TYPE_MARKER: return "Marker";
    case GL_DEBUG_TYPE_PUSH_GROUP: return "PushGroup";
    case GL_DEBUG_TYPE_POP_GROUP: return "PopGroup";
    case GL_DEBUG_TYPE_OTHER: return "Other";
    default: return "Unknown";
    }
}

inline const char* SeverityToStr(GLenum severity) {
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
    case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
    case GL_DEBUG_SEVERITY_LOW: return "LOW";
    case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFY";
    default: return "Unknown";
    }
}

inline const char* ErrorToStr(GLenum err) {
    switch (err) {
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    default: return "Unknown";
    }
}

inline bool HasDebugBit() {
    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    return (flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0;
}

inline void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                    const GLchar* message, const void* userParam) {
    (void)length;
    (void)userParam;

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

    std::cerr << "[GL][" << SeverityToStr(severity) << "] " << SourceToStr(source) << "/" << TypeToStr(type)
              << " (id=" << id << "): " << (message ? message : "") << std::endl;
}

inline void SetupDebugOutput() {
#if defined(GL_VERSION_4_6) || defined(GLAD_GL_KHR_debug)
    if (!HasDebugBit()) {
        std::cerr << "[GL] Debug context not enabled (no DEBUG_BIT)." << std::endl;
        return;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback(DebugCallback, nullptr);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

    std::cerr << "[GL] KHR_debug callback enabled." << std::endl;
#else
    std::cerr << "[GL] KHR_debug not available in this build of glad." << std::endl;
#endif
}

inline void Checkpoint(const char* label, const char* file, int line) {
    GLenum err = GL_NO_ERROR;
    bool any = false;
    while ((err = glGetError()) != GL_NO_ERROR) {
        any = true;
        std::cerr << "[GL][GetError] " << ErrorToStr(err) << " (0x" << std::hex << err << std::dec << ")"
                  << " at " << (file ? file : "") << ":" << line
                  << " | " << (label ? label : "") << std::endl;
    }

    (void)any;
}

inline void LabelObject(GLenum identifier, GLuint name, const char* label) {
#if defined(GL_VERSION_4_6) || defined(GLAD_GL_KHR_debug)
    if (!label || name == 0) {
        return;
    }
    if (!HasDebugBit()) {
        return;
    }
    glObjectLabel(identifier, name, -1, label);
#else
    (void)identifier;
    (void)name;
    (void)label;
#endif
}

struct Scope {
    Scope(const char* label, const char* file, int line)
        : m_label(label), m_file(file), m_line(line) {
#if defined(GL_VERSION_4_6) || defined(GLAD_GL_KHR_debug)
        if (m_label && HasDebugBit()) {
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, m_label);
        }
#else
        (void)m_label;
        (void)m_file;
        (void)m_line;
#endif
    }

    ~Scope() {
#if defined(GL_VERSION_4_6) || defined(GLAD_GL_KHR_debug)
        if (m_label && HasDebugBit()) {
            glPopDebugGroup();
        }
#endif
    }

    const char* m_label;
    const char* m_file;
    int m_line;
};

#else

inline void SetupDebugOutput() {}
inline void Checkpoint(const char*, const char*, int) {}
inline void LabelObject(GLenum, GLuint, const char*) {}
struct Scope {
    Scope(const char*, const char*, int) {}
};

#endif
} // namespace GLDebug

#ifdef DEBUG
#define ML_GL_CHECKPOINT(label) ::GLDebug::Checkpoint((label), __FILE__, __LINE__)
#define ML_GL_LABEL(identifier, name, label) ::GLDebug::LabelObject((identifier), (name), (label))
#define ML_GL_SCOPE(label) ::GLDebug::Scope ML_GL_SCOPE_##__LINE__{(label), __FILE__, __LINE__}
#else
#define ML_GL_CHECKPOINT(label) ((void)0)
#define ML_GL_LABEL(identifier, name, label) ((void)0)
#define ML_GL_SCOPE(label) ((void)0)
#endif
