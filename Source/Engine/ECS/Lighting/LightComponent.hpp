#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/ECS/IComponent.hpp"

class LightComponent : public IComponent
{
public:
	LightComponent(const char* uniform)
		: color{ 1.0f, 1.0f, 1.0f }, /* default white color */
			ambient{ 0.125f },
			diffuse{ 0.25f },
			specular{ 0.25f }
	{
		this->uniform.reserve(64);					/* Pre allocate memory */
		this->uniform = uniform;
	}

	/* Return following string representation:
		"color=<color.x,color.y,color.z>"
		"ambient=<ambient>"
		"diffuse=<diffuse>"
		"specular=<specular>"
	*/
	void ToString(String& out) const override
	{
		char buff[64]{};
		sprintf_s(buff, "color=%.3f,%.3f,%.3f\n", color.x, color.y, color.z);
		out.append(buff);
		sprintf_s(buff, "ambient=%.3f\n", ambient);
		out.append(buff);
		sprintf_s(buff, "diffuse=%.3f\n", diffuse);
		out.append(buff);
		sprintf_s(buff, "specular=%.3f\n", specular);
		out.append(buff);
	}

	virtual void Render(class Shader* shader) {}

	Vec3f color;    /* light color */
	float ambient;  /* ambient intensity */
	float diffuse;  /* diffuse intensity */
	float specular; /* specular intensity */

	String uniform;	/* Used in shader */
};