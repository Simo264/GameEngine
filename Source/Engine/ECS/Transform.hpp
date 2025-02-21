#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

/**
 * @brief
 * Represents the GameObject's transformation (location, rotation, scale) in world space
 */
class Transform
{
public:
	Transform();
	~Transform() = default;

	/** @brief Update the transformation matrix. Call this function after changing position, scale or rotation. */
	void UpdateTransformation();

	/** @return The transformation matrix */
	mat4f& GetTransformation() { return _transformation; }

	vec3f position;
	vec3f scale;
	vec3f rotation;	/* In degrees */
	
private:
	mat4f _transformation;
};