#include "material.h"

void Material::bind() const
{
    m_shader.use();
    for (const auto& [name, value] : m_uniformValues) {
        std::visit([&](const auto& val) { m_shader.setUniform(name, val); }, value);
    }
    for (const auto& [name, binding] : m_textureBindings) {
        if (binding.texture) {
            binding.texture->bind(binding.slot);
            m_shader.setUniform(name, static_cast<int>(binding.slot));
        }
    }
}

void Material::clear()
{
    m_uniformValues.clear();
    m_textureBindings.clear();
}

void Material::eraseUniform(std::string_view name)
{
    m_uniformValues.erase(std::string(name));
}