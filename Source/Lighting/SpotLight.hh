#pragma once

#include "PointLight.hh"

namespace Lighting
{
  // A spotlight is a light source that is located somewhere in the environment that, 
  // instead of shooting light rays in all directions, only shoots them in a specific direction.
  // A good example of a spotlight would be a street lamp or a flashlight. 
  class SpotLight : public PointLight
  {
  public:
    SpotLight(string uniformName);

    void Render(Graphics::Shader* shader);

    vec3f direction;

    float cutOff; // the cutoff angle that specifies the spotlight's radius.
  };

}