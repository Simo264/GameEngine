#pragma once

#include "Core/Math/Math.hpp"

/* ----------------------------------------------------------------------
	This component defines the transformation (location, rotation, scale) 
	of an object in world
 ------------------------------------------------------------------------ */
class TransformComponent
{
public:
	TransformComponent() : 
		position{ 0,0,0 }, 
		scale{ 1,1,1 }, 
		degrees{ 0,0,0 }
	{}

	Vec3f position;
	Vec3f degrees;	/* rotation in degree */
	Vec3f scale;

	/* Calculate the model matrix */
	Mat4f GetTransformation() const
	{
		static const Mat4f I = Mat4f(1.0f);

		Mat4f translation = glm::translate(I, position);
		Mat4f rotation =
			glm::rotate(I, glm::radians(degrees.x), Vec3f(1.0f, 0.0f, 0.0f)) *
			glm::rotate(I, glm::radians(degrees.y), Vec3f(0.0f, 1.0f, 0.0f)) *
			glm::rotate(I, glm::radians(degrees.z), Vec3f(0.0f, 0.0f, 1.0f));
		Mat4f scaling = glm::scale(I, scale);

		return translation * rotation * scaling;
	}
};