#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/ECS/Lighting/LightComponent.hpp"

/* ------------------------------------------------------------------------------
  A point light is a light source with a given position somewhere in a world
  that illuminates in all directions, where the light rays fade out over distance.
  Think of light bulbs and torches as light casters that act as a point light.
  ------------------------------------------------------------------------------ */
class PointLightComponent : public ILightComponent
{
public:
	PointLightComponent(const char* uniform)
    : ILightComponent(uniform),
      position{ 0.0f, 0.0f, 0.0f },
      linear{ 0.14f },
      quadratic{ 0.07f }
  {}

  void Render(class Shader* shader) override;

  Vec3f position;

  /* Attenuation */
  float linear;
  float quadratic;
};
