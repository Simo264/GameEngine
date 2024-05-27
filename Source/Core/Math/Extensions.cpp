#include "Extensions.hpp"

// Include all GLM extensions
#include <glm/ext.hpp>									// perspective, translate, rotate
#include <glm/gtx/matrix_decompose.hpp> // decompose


namespace Math
{
	mat4f Perspective(float fovy, float aspect, float zNear, float zFar)
	{
		return glm::perspective(fovy, aspect, zNear, zFar);
	}

	mat4f Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		return glm::ortho(left, right, bottom, top, zNear, zFar);
	}

	void Decompose(const mat4f& model, vec3f& translation, Quat& rotation, vec3f& scale)
	{
		translation = model[3];
		for (int i = 0; i < 3; i++)
			scale[i] = Math::Length(vec3f(model[i]));

		const mat3f rotMtx(
			vec3f(model[0]) / scale[0],
			vec3f(model[1]) / scale[1],
			vec3f(model[2]) / scale[2]);

		rotation = QuatCast(rotMtx);
	}

	Quat Conjugate(Quat& rotation)
	{
		return glm::conjugate(rotation);
	}

	mat4f Translate(const mat4f& model, const vec3f& position)
	{
		return glm::translate(model, position);
	}

	mat4f Scale(const mat4f& model, const vec3f& scaling)
	{
		return glm::scale(model, scaling);
	}

	mat4f Rotate(const mat4f& model, float radians, const vec3f& axis)
	{
		return glm::rotate(model, radians, axis);
	}

	mat4f LookAt(const vec3f& eye, const vec3f& center, const vec3f& up)
	{
		return glm::lookAt(eye, center, up);
	}

	Quat QuatCast(const mat3f& m)
	{
		return glm::quat_cast(m);
	}

	vec3f EulerAngles(const Quat& q)
	{
		return glm::eulerAngles(q);
	}
}
