#pragma once

#include "Mesh/Mesh.hh"

// Base class for the every object places in the world
// ----------------------------------------------------
class Actor : public Mesh
{
public:
	Actor();
	~Actor() = default;

	mat4f GetModel();
	
	void Draw(Shader* shader);

	vec3f position;
	vec3f scaling;
	vec3f rotation;
	float rotationAngle;

	string actorName;

	inline static int numActors = 0;
};

