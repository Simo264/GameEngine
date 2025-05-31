#pragma once

#include "Core/Core.hpp"

namespace UniformLocations
{
	constexpr i32 MODEL = 0;							// layout(location = 0) uniform mat4 u_model;
}

namespace UniformBindings
{
	constexpr i32 CAMERA_BLOCK = 0;				// layout(std140, binding = 0) uniform CameraBlock
	constexpr i32 LIGHTING_BLOCK = 1;			// layout(std140, binding = 1) uniform LightingBlock
	constexpr i32 BONES_BLOCK = 2;				// layout(std140, binding = 2) uniform BonesBlock

	constexpr i32 TEXTURE_COLOR = 10;			// layout(binding = 10) uniform sampler2D u_textureColor;
	constexpr i32 TEXTURE_SPECULAR = 11;	// layout(binding = 11) uniform sampler2D u_textureSpecular;
	constexpr i32 TEXTURE_NORMAL = 12;		// layout(binding = 12) uniform sampler2D u_textureNormal;
}