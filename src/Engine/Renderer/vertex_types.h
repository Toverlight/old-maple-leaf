#pragma once

#include <cstddef>
#include <type_traits>

#include "Engine/RHI/vertex_layout.h"

// A simple, tightly-packed vertex format for early rendering.
// Fields are explicit floats to avoid surprises from library-specific alignment.
struct VertexPCU
{
    float px, py, pz;
    float cr, cg, cb;
    float u, v;

    static VertexLayout Layout()
    {
        static_assert(std::is_standard_layout_v<VertexPCU>, "VertexPCU must be standard layout for offsetof");

        VertexLayout layout;
        layout.addFloat(0, 3, static_cast<std::uint32_t>(offsetof(VertexPCU, px)), static_cast<GLsizei>(sizeof(VertexPCU)));
        layout.addFloat(1, 3, static_cast<std::uint32_t>(offsetof(VertexPCU, cr)), static_cast<GLsizei>(sizeof(VertexPCU)));
        layout.addFloat(2, 2, static_cast<std::uint32_t>(offsetof(VertexPCU, u)),  static_cast<GLsizei>(sizeof(VertexPCU)));
        return layout;
    }
};

struct VertexPU
{
    float px, py, pz;
    float u, v;

    static VertexLayout Layout()
    {
        static_assert(std::is_standard_layout_v<VertexPU>, "VertexPU must be standard layout for offsetof");

        VertexLayout layout;
        layout.addFloat(0, 3, static_cast<std::uint32_t>(offsetof(VertexPU, px)), static_cast<GLsizei>(sizeof(VertexPU)));
        layout.addFloat(1, 2, static_cast<std::uint32_t>(offsetof(VertexPU, u)),  static_cast<GLsizei>(sizeof(VertexPU)));
        return layout;
    }
};

struct VertexPNCU
{
    float px, py, pz;
    float nx, ny, nz;
    float cr, cg, cb;
    float u, v;

    static VertexLayout Layout()
    {
        static_assert(std::is_standard_layout_v<VertexPNCU>, "VertexPNCU must be standard layout for offsetof");

        VertexLayout layout;
        layout.addFloat(0, 3, static_cast<std::uint32_t>(offsetof(VertexPNCU, px)), static_cast<GLsizei>(sizeof(VertexPNCU)));
        layout.addFloat(1, 3, static_cast<std::uint32_t>(offsetof(VertexPNCU, nx)), static_cast<GLsizei>(sizeof(VertexPNCU)));
        layout.addFloat(2, 3, static_cast<std::uint32_t>(offsetof(VertexPNCU, cr)), static_cast<GLsizei>(sizeof(VertexPNCU)));
        layout.addFloat(3, 2, static_cast<std::uint32_t>(offsetof(VertexPNCU, u)),  static_cast<GLsizei>(sizeof(VertexPNCU)));
        return layout;
    }
};

struct VertexPC
{
    float px, py, pz;
    float cr, cg, cb;

    static VertexLayout Layout()
    {
        static_assert(std::is_standard_layout_v<VertexPC>, "VertexPC must be standard layout for offsetof");

        VertexLayout layout;
        layout.addFloat(0, 3, static_cast<std::uint32_t>(offsetof(VertexPC, px)), static_cast<GLsizei>(sizeof(VertexPC)));
        layout.addFloat(1, 3, static_cast<std::uint32_t>(offsetof(VertexPC, cr)), static_cast<GLsizei>(sizeof(VertexPC)));
        return layout;
    }
};