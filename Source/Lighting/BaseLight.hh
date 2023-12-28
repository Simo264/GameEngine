#pragma once

#include "../Core.hh"
#include "../Graphics/Shader.hh"

// Interface lighting class
// ----------------------------
class BaseLight
{
public:
  BaseLight(const char* uniformName)
  {
    this->uniformName.reserve(50); // Pre-allocation memory
    this->uniformName.append(uniformName);

    color     = vec3f(1.0f, 1.0f, 1.0f);  // default white color
    ambient   = 0.25f;                    // default ambient intensity
    diffuse   = 0.50f;                    // default diffuse intensity
    specular  = 0.75f;                    // default specular intensity
  }
  ~BaseLight() = default;

  virtual void Render(Shader* shader) = 0;

  vec3f color;    // light color
  float ambient;  // ambient intensity
  float diffuse;  // diffuse intensity
  float specular; // specular intensity

  string uniformName;
};
