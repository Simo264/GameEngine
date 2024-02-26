#pragma once

#include "Core/Math/Math.hpp"

/* ----------------------------------------------------------------------
	This component defines the transform (location, rotation, scale) 
	of an Actor in world
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
	Mat4f GetTransformation() const;
};