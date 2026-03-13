#pragma once
#include <string_view>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <unordered_set>

class Shader
{
public:
    Shader(std::string_view vertexSource, std::string_view fragmentSource);
    ~Shader();

    GLuint ID() const { return m_programID; }
    void use() const;

    template<typename T>
    void setUniform(std::string_view name, const T& value) const {
        const GLint loc = getUniformLocation(name);
        if (loc == -1) {
            return;
        }
        
        if constexpr (std::is_same_v<T, int>)
            glUniform1i(loc, value);
        else if constexpr (std::is_same_v<T, float>)
            glUniform1f(loc, value);
        else if constexpr (std::is_same_v<T, glm::vec2>)
            glUniform2fv(loc, 1, glm::value_ptr(value));
        else if constexpr (std::is_same_v<T, glm::vec3>)
            glUniform3fv(loc, 1, glm::value_ptr(value));
        else if constexpr (std::is_same_v<T, glm::vec4>)
            glUniform4fv(loc, 1, glm::value_ptr(value));
        else if constexpr (std::is_same_v<T, glm::mat4>)
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        else
            static_assert(!sizeof(T), "Unsupported uniform type");
    }

private:
    GLint getUniformLocation(std::string_view name) const;
    void logMissingUniform(std::string_view name) const;

private:
    GLuint m_programID = 0;
    mutable std::unordered_map<std::string, GLint> m_uniformLocations;
    mutable std::unordered_set<std::string> m_missingUniformWarnings;
};