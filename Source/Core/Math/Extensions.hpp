#pragma once

#include "Core/Core.hpp"

#include "Math.hpp"

#include <glm/gtc/quaternion.hpp>

using quat = glm::quat;

namespace Math
{
	mat4f Perspective(f32 fovy, f32 aspect, f32 zNear, f32 zFar);
	mat4f Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);
	
	mat4f Translate(const mat4f& model, const vec3f& position);
	mat4f Scale(const mat4f& model, const vec3f& scaling);
	mat4f Rotate(const mat4f& model, f32 radians, const vec3f& axis);

	mat4f LookAt(const vec3f& eye, const vec3f& center, const vec3f& up);

	/* Decomposes the model matrix to translations,rotation scale components */
	void Decompose(const mat4f& model, vec3f& translation, quat& rotation, vec3f& scaling);
	
	quat Conjugate(quat& rotation);

	quat QuatCast(const mat3f& m);

	vec3f EulerAngles(const quat& q);
}

