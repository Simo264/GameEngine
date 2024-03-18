#include "Extensions.hpp"

// Include all GLM extensions
#include <glm/ext.hpp> // perspective, translate, rotate

#include <glm/gtx/matrix_decompose.hpp> // decompose


namespace Math
{
	Mat4f Perspective(float fovy, float aspect, float zNear, float zFar)
	{
		return glm::perspective(fovy, aspect, zNear, zFar);
	}

	Mat4f Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		return glm::ortho(left, right, bottom, top, zNear, zFar);
	}

	void Decompose(const Mat4f& model, Vec3f& translation, Quat& rotation, Vec3f& scale)
	{
		translation = model[3];
		for (int i = 0; i < 3; i++)
			scale[i] = Math::Length(Vec3f(model[i]));

		const Mat3f rotMtx(
			Vec3f(model[0]) / scale[0],
			Vec3f(model[1]) / scale[1],
			Vec3f(model[2]) / scale[2]);

		rotation = QuatCast(rotMtx);
	}

	Quat Conjugate(Quat& rotation)
	{
		return glm::conjugate(rotation);
	}

	Mat4f Translate(const Mat4f& model, const Vec3f& position)
	{
		return glm::translate(model, position);
	}

	Mat4f Scale(const Mat4f& model, const Vec3f& scaling)
	{
		return glm::scale(model, scaling);
	}

	Mat4f Rotate(const Mat4f& model, float radians, const Vec3f& axis)
	{
		return glm::rotate(model, radians, axis);
	}

	Mat4f LookAt(const Vec3f& eye, const Vec3f& center, const Vec3f& up)
	{
		return glm::lookAt(eye, center, up);
	}

	Quat QuatCast(const Mat3f& m)
	{
		return glm::quat_cast(m);
	}

	Vec3f EulerAngles(const Quat& q)
	{
		return glm::eulerAngles(q);
	}
}
