#pragma once

#include "PointLight.hh"

// A spotlight is a light source that is located somewhere in the environment that, 
// instead of shooting light rays in all directions, only shoots them in a specific direction.
// A good example of a spotlight would be a street lamp or a flashlight. 
class SpotLight : public PointLight
{
public:
  SpotLight(const char* uniformName) : PointLight(uniformName), direction{ 0.0f, -1.0f, 0.0f }, cutOff{ 12.0f }{ }

  void Render(Shader* shader);

  Vec3f direction;
  float cutOff; // the cutoff angle that specifies the spotlight's radius.
};

