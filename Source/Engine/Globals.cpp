#include "Core/OpenGL.hpp"
#include "Core/Math/Math.hpp"

float	Z_NEAR		=  0.1f;
float	Z_FAR			=  50.0f;
float LEFT			= -20.0f;
float RIGHT			=  20.0f;
float BOTTOM		= -20.0f;
float TOP				=  20.0f;

int DRAW_MODE = GL_TRIANGLES;

float AMBIENT_INTENSITY = 0.0f;
vec3f AMBIENT_COLOR = vec3f(1.0f, 1.0f, 1.0f);
