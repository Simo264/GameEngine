#pragma once

#include "Core/Math/Math.hpp"

/**
 * Vertex format with position
 */
struct Vertex_P
{
	vec3f position;
};

/**
 * Vertex format with position and uv coordinates
 */
struct Vertex_P_UV
{
	vec3f position;
	vec2f uv;
};

/**
 * Vertex format with position, normal, uv coordinates and tangent
 */
struct Vertex_P_N_UV_T
{
	vec3f position;
	vec3f normal;
	vec2f uv;
	vec3f tangent;
};