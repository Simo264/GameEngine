#pragma once

#include "Core/Math/Math.hpp"

constexpr int	WINDOW_WIDTH = 1600;
constexpr int	WINDOW_HEIGHT = 900;

extern float Z_NEAR;
extern float Z_FAR;
extern float LEFT;
extern float RIGHT;
extern float BOTTOM;
extern float TOP;

extern int DRAW_MODE;

extern float AMBIENT_INTENSITY;
extern vec3f AMBIENT_COLOR;

extern class WindowManager g_windowManager;
extern class ShaderManager g_shaderManager;
extern class TextureManager g_textureManager;