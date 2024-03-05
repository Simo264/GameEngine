#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "PointLightComponent.hpp"

/* ------------------------------------------------------------------------------
  A spotlight is a light source that is located somewhere in the environment that,
  instead of shooting light rays in all directions, only shoots them in a specific direction.
  A good example of a spotlight would be a street lamp or a flashlight.
  ------------------------------------------------------------------------------ */
class SpotLightComponent : PointLightComponent
{
public:
  SpotLightComponent(const char* uniform)
    : PointLightComponent(uniform),
    direction{ 0.0f, -1.0f, 0.0f },
    cutOff{ 12.0f }
  {}
  
	/* Return following string representation:
	  "color=<color.x,color.y,color.z>"
	  "ambient=<ambient>"
	  "diffuse=<diffuse>"
	  "direction=<direction.x,direction.y,direction.z>"
    "cutoff=<cutoff>"
  */
  void ToString(String& out) const override
	{
    LightComponent::ToString(out);
    out.append("\n");

    char buff[64]{};
    sprintf_s(buff, "direction=%.3f,%.3f,%.3f\n", direction.x, direction.y, direction.z);
    out.append(buff);

    sprintf_s(buff, "cutoff=%.3f\n", cutOff);
    out.append(buff);
	}

  void Render(class Shader* shader) override;

	Vec3f direction;
  
  float cutOff; /* the cutoff angle that specifies the spotlight's radius. */
};