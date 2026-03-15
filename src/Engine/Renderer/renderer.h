#pragma once

#include <span>

class Renderer
{
public:
    static void Clear();
    static void Submit(const class Mesh& mesh, std::span<const class Material> materials);
};