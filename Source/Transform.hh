#pragma once

#include "Core.hh"

/* ----------------------------------------------------------------------
	This component defines the transform (location, rotation, scale) 
	of this Actor in the world
 ------------------------------------------------------------------------ */
class Transform
{
public:
	Transform() : 
		position{ 0,0,0 }, 
		scale{ 1,1,1 }, 
		degrees{ 0,0,0 }
	{}

	Vec3f position;
	Vec3f degrees;	/* rotation in degree */
	Vec3f scale;

	/* Calculate the model matrix */
	Mat4f GetTransformation()
	{
		Mat4f model = Mat4f(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(degrees.x), Vec3f(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(degrees.y), Vec3f(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(degrees.z), Vec3f(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);
		return model;
	}
};