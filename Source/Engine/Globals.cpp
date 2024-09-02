#include "Core/OpenGL.hpp"
#include "Core/Math/Math.hpp"

u32 g_drawCalls = 0;

f32 g_ambientIntensity = 0.0f;
vec3f g_ambientColor = vec3f(1.0f, 1.0f, 1.0f);