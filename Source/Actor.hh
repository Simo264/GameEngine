#pragma once

#include "Core.hh"

class Actor
{
public:
	Actor();
	~Actor() = default;

	mat4f Model();

	vec3f position;
	vec3f scaling;
	vec3f rotation;
	float rotationAngle;
};

