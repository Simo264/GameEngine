#include "Core/OpenGL.hpp"
#include "Core/Math/Math.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

int g_drawMode = GL_TRIANGLES;
uint32_t g_drawCalls = 0;

float g_ambientIntensity = 0.0f;
vec3f g_ambientColor = vec3f(1.0f, 1.0f, 1.0f);

WindowManager g_windowManager;
ShaderManager g_shaderManager;
TextureManager g_textureManager;