#pragma once

#include "Core/Math/Math.hpp"

constexpr i32	WINDOW_WIDTH = 1600;
constexpr i32	WINDOW_HEIGHT = 900;

extern u32 g_drawCalls;

extern f32 g_ambientIntensity;
extern vec3f g_ambientColor;

extern class WindowManager g_windowManager;
extern class ShaderManager g_shaderManager;
extern class TextureManager g_textureManager;