#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/ECS/Lighting/LightComponent.hpp"

/* ------------------------------------------------------------------------------
  A point light is a light source with a given position somewhere in a world
  that illuminates in all directions, where the light rays fade out over distance.
  Think of light bulbs and torches as light casters that act as a point light.
  ------------------------------------------------------------------------------ */
class PointLightComponent : public LightComponent
{
public:
	PointLightComponent(const char* uniform)
    : LightComponent(uniform),
      position{ 0.0f, 0.0f, 0.0f },
      linear{ 0.14f },
      quadratic{ 0.07f }
  {}

  /* Return following string representation:
    "color=<color.x,color.y,color.z>"
    "ambient=<ambient>"
    "diffuse=<diffuse>"
    "specular=<specular>"
    "position=<position.x,position.y,position.z>"
  */
  void ToString(String& out) const override
  {
    LightComponent::ToString(out);
    out.append("\n");

    char buff[64]{};
    sprintf_s(buff, "position=%.3f,%.3f,%.3f\n", position.x, position.y, position.z);
    out.append(buff);
  }

  void Render(class Shader* shader) override;

  Vec3f position;

  /* Attenuation */
  float linear;
  float quadratic;
};
