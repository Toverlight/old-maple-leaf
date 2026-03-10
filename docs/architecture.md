# 引擎代码架构

```txt
src/
├── main.cpp
├── Editor/                      （未来编辑器 UI）
└── Engine/
    ├── Core/                    ← 底层工具，无渲染概念
    │   ├── shader.h / .cpp      （GPU 程序）
    │   └── ...（未来：logger、event、timer）
    │
    ├── RHI/                     ← Rendering Hardware Interface（GL 对象的薄封装）
    │   ├── vertex_array.h/.cpp  （VAO）
    │   ├── vertex_buffer.h/.cpp （VBO）
    │   ├── index_buffer.h/.cpp  （EBO，未来需要）
    │   └── texture.h/.cpp       （Texture，未来需要）
    │
    ├── Renderer/                ← 高层渲染逻辑
    │   ├── renderer.h/.cpp      （提交 draw call，管理渲染状态）
    │   ├── mesh.h/.cpp          （组合 RHI 对象 + 顶点布局描述）
    │   └── material.h/.cpp      （持有 Shader + uniform 参数集合）
    │
    └── Scene/                   （未来：Entity、Component、Camera）

```
