#include "Transform.hpp"

Mat4f Transform::GetTransformation() const
{
	const Mat4f I = Mat4f(1.0f);

	Mat4f translation = glm::translate(I, position);
	Mat4f rotation =
		glm::rotate(I, glm::radians(degrees.x), Vec3f(1.0f, 0.0f, 0.0f)) *
		glm::rotate(I, glm::radians(degrees.y), Vec3f(0.0f, 1.0f, 0.0f)) *
		glm::rotate(I, glm::radians(degrees.z), Vec3f(0.0f, 0.0f, 1.0f));
	Mat4f scaling = glm::scale(I, scale);

	return translation * rotation * scaling;
}