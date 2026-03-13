#pragma once

class Renderer
{
public:
    static void Clear();
    static void Submit(const class Mesh& mesh, const class Material& material);
};