#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

/* -------------------------------------
      BaseLight interface class 
  ------------------------------------- */
class BaseLight
{
public:
  BaseLight(const char* uniformName)
  {
    this->_uniformName.reserve(50); /* Pre allocate memory for 50 chars */
    this->_uniformName.append(uniformName);

    color     = Vec3f(1.0f, 1.0f, 1.0f);  /* default white color */ 
    ambient   = 0.125f;                   /* default ambient intensity */
    diffuse   = 0.25f;                    /* default diffuse intensity */
    specular  = 0.25f;                    /* default specular intensity */
  }

  virtual void RenderLight(class Shader* shader) = 0;

  Vec3f color;    /* light color */ 
  float ambient;  /* ambient intensity */
  float diffuse;  /* diffuse intensity */
  float specular; /* specular intensity */

protected:
  String _uniformName;
};
