#pragma once

#include "Core.hh"


/* ----------------------------------------------------------------------
	The component that defines the transform(location, rotation, scale) 
	of this Actor in the world,
	all other components must be attached to this one somehow
 ------------------------------------------------------------------------ */
class RootComponent
{
public:
	RootComponent() 
		: position{ Vec3f(0,0,0) }, scaling{ Vec3f(1,1,1) }, rotationAxis{ Vec3f(1,0,0) }, rotationAngle{ 0 } {}

	Vec3f position;
	Vec3f scaling;
	Vec3f rotationAxis;
	float rotationAngle;

	const Mat4f GetModel() const
	{
		Mat4f model = Mat4f(1.0f);
		model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
		model = glm::translate(model, position);
		model = glm::scale(model, scaling);
		return model;
	}
};