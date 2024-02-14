#pragma once

#include "../Core.hh"
#include "../SceneObject.hh"

/* -------------------------------------
      Interface lighting class 
  ------------------------------------- */
class BaseLight : public SceneObject
{
public:
  BaseLight(const char* uniformName)
  {
    this->_uniformName.reserve(50); /* Pre allocate memory for 50 chars */
    this->_uniformName.append(uniformName);

    color     = Vec3f(1.0f, 1.0f, 1.0f);  /* default white color */ 
    ambient   = 0.125f;                    /* default ambient intensity */
    diffuse   = 0.25f;                    /* default diffuse intensity */
    specular  = 0.25f;                    /* default specular intensity */
    
    this->tagName = "Light";
  }

  virtual void RenderLight(class Shader* shader) = 0;

  Vec3f color;    /* light color */ 
  float ambient;  /* ambient intensity */
  float diffuse;  /* diffuse intensity */
  float specular; /* specular intensity */

  const char* GetUniformName() const { return _uniformName.c_str(); }

protected:
  String _uniformName;
};
