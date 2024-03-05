#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/ECS/Lighting/LightComponent.hpp"

/* ------------------------------------------------------------------------------------------------
	When a light source is modeled to be infinitely far away it is called a directional light
	since all its light rays have the same direction;
	it is independent of the location of the light source.
	A fine example of a directional light source is the sun as we know it.
	The sun is not infinitely far away from us, but it is so far away that we can perceive it as
	being infinitely far away in the lighting calculations
	------------------------------------------------------------------------------------------------ */
class DirLightComponent : public LightComponent
{
public:
	DirLightComponent(const char* uniform)
		: LightComponent(uniform),
			direction{ 0.0f, -1.0f, 0.0f }
	{}
	
	/* Return following string representation:
	"color=<color.x,color.y,color.z>"
	"ambient=<ambient>"
	"diffuse=<diffuse>"
	"specular=<specular>"
	"direction=<direction.x,direction.y,direction.z>"
*/
	void ToString(String& out) const override
	{
		LightComponent::ToString(out);

		char buff[64]{};
		sprintf_s(buff, "direction=%.3f,%.3f,%.3f\n", direction.x, direction.y, direction.z);
		out.append(buff);
	}

	void Render(class Shader* shader) override;

	Vec3f direction;
};
