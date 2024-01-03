#pragma once

#include "Core.hh"

// The component that defines the transform (location, rotation, scale) of this Actor in the world, 
// all other components must be attached to this one somehow
// ----------------------------------------------------------------------------------
class RootComponent
{

public:
	RootComponent() 
		: position{ vec3f(0,0,0) }, scaling{ vec3f(1,1,1) }, rotationAxis{ vec3f(1,1,1) }, rotationAngle{ 0 } {}

	vec3f position;
	vec3f scaling;
	vec3f rotationAxis;
	float rotationAngle;

	const mat4f GetModel() const
	{
		mat4f model = mat4f(1.0f);
		model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
		model = glm::translate(model, position);
		model = glm::scale(model, scaling);
		return model;
	}
};