#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

/**
 * Represents the GameObject's transformation (location, rotation, scale) in world space
 */
class Transform
{
public:
	Transform();
	~Transform() = default;

	/**
	 * Update the transformation matrix. Call this function after changing position, scale or rotation.
	 */
	void UpdateTransformation();

	/**
	 * Calculate the model matrix
	 */
	constexpr mat4f& GetTransformation() { return _transformation; }

	vec3f position;
	vec3f scale;
	vec3f rotation;	/* In degrees */

private:
	mat4f _transformation;
};