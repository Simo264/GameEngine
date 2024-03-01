#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

class LightComponent
{
public:
	LightComponent(const char* uniform)
		: color{ 1.0f, 1.0f, 1.0f }, /* default white color */
			ambient{ 0.125f },
			diffuse{ 0.25f },
			specular{ 0.25f }
	{
		this->uniform.reserve(64);					/* Pre allocate memory */
		this->uniform.append(uniform);
	}

	virtual void Render(class Shader* shader) = 0;

	Vec3f color;    /* light color */
	float ambient;  /* ambient intensity */
	float diffuse;  /* diffuse intensity */
	float specular; /* specular intensity */

	String uniform;	/* Used in shader */
};