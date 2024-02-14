#pragma once

#include "BaseLight.hh"

struct Attenuation
{
  Attenuation(float linear = 0.14f, float quadratic = 0.07f) :
    linear{ linear },
    quadratic{ quadratic }
  {}

  float linear;
  float quadratic;
};

/* ------------------------------------------------------------------------------
  A point light is a light source with a given position somewhere in a world 
  that illuminates in all directions, where the light rays fade out over distance.
  Think of light bulbs and torches as light casters that act as a point light.
  ------------------------------------------------------------------------------ */
class PointLight : public BaseLight
{
public:
  PointLight(const char* uniformName) : 
    BaseLight(uniformName), 
    position{ 0.0f, 0.0f, 0.0f } 
  {
    this->tagName = "Point light";
  }
    
  void RenderLight(class Shader* shader) override;

  Vec3f position;
  Attenuation attenuation;
};

