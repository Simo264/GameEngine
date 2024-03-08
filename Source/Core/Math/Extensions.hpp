#pragma once

#include "Math.hpp"

#include <glm/gtc/quaternion.hpp>

using Quat = glm::quat;

namespace Math
{
	Mat4f Perspective(float fovy, float aspect, float zNear, float zFar);
	
	Mat4f Translate(const Mat4f& model, const Vec3f& position);
	Mat4f Scale(const Mat4f& model, const Vec3f& scaling);
	Mat4f Rotate(const Mat4f& model, float radians, const Vec3f& axis);

	Mat4f LookAt(const Vec3f& eye, const Vec3f& center, const Vec3f& up);

	/* Decomposes the model matrix to translations,rotation scale components */
	void Decompose(const Mat4f& model, Vec3f& translation, Quat& rotation, Vec3f& scaling);
	
	Quat Conjugate(Quat& rotation);

	Quat QuatCast(const Mat3f& m);

	Vec3f EulerAngles(const Quat& q);
}

