#pragma once

#include "Math.hpp"

#include <glm/gtc/quaternion.hpp>

using Quat = glm::quat;

namespace Math
{
	mat4f Perspective(float fovy, float aspect, float zNear, float zFar);
	mat4f Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
	
	mat4f Translate(const mat4f& model, const vec3f& position);
	mat4f Scale(const mat4f& model, const vec3f& scaling);
	mat4f Rotate(const mat4f& model, float radians, const vec3f& axis);

	mat4f LookAt(const vec3f& eye, const vec3f& center, const vec3f& up);

	/* Decomposes the model matrix to translations,rotation scale components */
	void Decompose(const mat4f& model, vec3f& translation, Quat& rotation, vec3f& scaling);
	
	Quat Conjugate(Quat& rotation);

	Quat QuatCast(const mat3f& m);

	vec3f EulerAngles(const Quat& q);
}

