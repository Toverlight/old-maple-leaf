#include "material.h"

void Material::bind() const
{
    m_shader.use();
    for (const auto& [name, value] : m_uniformValues) {
        std::visit([&](const auto& val) { m_shader.setUniform(name, val); }, value);
    }
}

void Material::clear()
{
    m_uniformValues.clear();
}

void Material::eraseUniform(std::string_view name)
{
    m_uniformValues.erase(std::string(name));
}