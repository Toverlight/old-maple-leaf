#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Engine/Core/shader.h"

using UniformValue = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

class Material
{
public:
    explicit Material(Shader& shader) : m_shader(shader) {}

    Shader& getShader() const { return m_shader; }

    template<typename T>
    void set(std::string_view name, T value)
    {
        m_uniformValues[std::string(name)] = value;
    }

    void bind() const;
    void clear();
    void eraseUniform(std::string_view name);

private:
    Shader& m_shader;
    std::unordered_map<std::string, UniformValue> m_uniformValues;
};