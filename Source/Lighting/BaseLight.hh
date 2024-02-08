#pragma once

#include "../Core.hh"
#include "../NonCopyable.hh"

/* -------------------------------------
      Interface lighting class 
  ------------------------------------- */
class BaseLight : public NonCopyable
{
public:
  BaseLight(const char* uniformName)
  {
    this->_uniformName.reserve(50); /* Pre allocate memory for 50 chars */
    this->_uniformName.append(uniformName);

    color     = Vec3f(1.0f, 1.0f, 1.0f);  /* default white color */ 
    ambient = 0.25f;                      /* default ambient intensity */
    diffuse = 0.50f;                      /* default diffuse intensity */
    specular = 0.75f;                     /* default specular intensity */
  }
  ~BaseLight() = default;

  virtual void RenderLight(class Shader* shader) = 0;

  Vec3f color;    /* light color */ 
  float ambient;  /* ambient intensity */
  float diffuse;  /* diffuse intensity */
  float specular; /* specular intensity */

protected:
  String _uniformName;
};
