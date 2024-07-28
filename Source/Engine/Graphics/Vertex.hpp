#pragma once

#include "Core/Math/Math.hpp"

struct Vertex
{
	vec3f position;
	vec2f texCoord;
	vec3f normal;
	vec3f tangent;
};

/* Number of vertex components */
constexpr int VERTEX_SIZE =
	3 +
	2 +
	3 +
	3;