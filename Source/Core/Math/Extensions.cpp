#include "Extensions.hpp"

/* Include all GLM extensions */ 
#include <glm/ext.hpp>									/* perspective, translate, rotate */ 
#include <glm/gtx/matrix_decompose.hpp> /* decompose */ 


namespace Math
{
	mat4f Perspective(f32 fovy, f32 aspect, f32 zNear, f32 zFar)
	{
		return glm::perspective(fovy, aspect, zNear, zFar);
	}

	mat4f Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
	{
		return glm::ortho(left, right, bottom, top, zNear, zFar);
	}

	void Decompose(const mat4f& model, vec3f& translation, quat& rotation, vec3f& scale)
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

	quat Conjugate(quat& rotation)
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

	mat4f Rotate(const mat4f& model, f32 radians, const vec3f& axis)
	{
		return glm::rotate(model, radians, axis);
	}

	mat4f LookAt(const vec3f& eye, const vec3f& center, const vec3f& up)
	{
		return glm::lookAt(eye, center, up);
	}

	quat QuatCast(const mat3f& m)
	{
		return glm::quat_cast(m);
	}

	vec3f EulerAngles(const quat& q)
	{
		return glm::eulerAngles(q);
	}
}
