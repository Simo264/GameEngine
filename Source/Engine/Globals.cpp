#include "Core/Core.hpp"

u32 g_drawCalls = 0;

// 0: Blinn-Phong shading
// 1: Gooch shading
i32 g_activeShadingModel = 0;

// 0: render color
// 1: render depth
// 2: render normals
i32 g_renderMode = 1;

bool g_renderInWireframe = false;
bool g_renderWithNormalMapping = true;