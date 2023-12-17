#pragma once

#include "../Core.hh"

namespace Lighting
{
  class BaseLight
  {
  public:
    BaseLight()
    {
      color     = vec3f(1.0f, 1.0f, 1.0f);  // default white color
      ambient   = 0.25f;                    // default ambient intensity
      diffuse   = 0.50f;                    // default diffuse intensity
      specular  = 0.75f;                    // default specular intensity
    }
    ~BaseLight() = default;

    vec3f color;    // light color
    float ambient;  // ambient intensity
    float diffuse;  // diffuse intensity
    float specular; // specular intensity
  };
}