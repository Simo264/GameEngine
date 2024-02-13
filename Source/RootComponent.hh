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
		: position{ Vec3f(0,0,0) }, 
		scaling{ Vec3f(1,1,1) }, 
		angleDegreeX{ 0 },
		angleDegreeY{ 0 },
		angleDegreeZ{ 0 } 
	{}

	Vec3f position;
	Vec3f scaling;
	float angleDegreeX;
	float angleDegreeY;
	float angleDegreeZ;

	const Mat4f GetModel() const
	{
		Mat4f model = Mat4f(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(angleDegreeX), Vec3f(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(angleDegreeY), Vec3f(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(angleDegreeZ), Vec3f(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scaling);
		return model;
	}
};