#pragma once

#include "Core/Math/Math.hpp"

constexpr int	WINDOW_WIDTH = 1600;
constexpr int	WINDOW_HEIGHT = 900;

extern uint32_t g_drawCalls;

extern float g_ambientIntensity;
extern vec3f g_ambientColor;

extern class WindowManager g_windowManager;
extern class ShaderManager g_shaderManager;
extern class TextureManager g_textureManager;