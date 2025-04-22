#pragma once

extern u32 g_drawCalls;

// 0: Blinn-Phong shading
// 1: Gooch shading
extern i32 g_activeShadingModel;

// 0: render color
// 1: render depth
// 2: render normals
extern i32 g_renderMode;

extern bool g_renderInWireframe;
extern bool g_renderWithNormalMapping;

