#include "PointLight.hpp"
#include "Engine/Shader.hpp"

void PointLight::RenderLight(Shader* shader)
{
  const uint64_t uniformNameSize = _uniformName.size();
  
  _uniformName.append(".position");                            /* shaderUName = "PointLight.position" */
  shader->SetVec3f(_uniformName.c_str(), position);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".ambient");                             /* shaderUName = "PointLight.ambient" */
  shader->SetVec3f(_uniformName.c_str(), color * ambient);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".diffuse");                             /* shaderUName = "PointLight.diffuse" */
  shader->SetVec3f(_uniformName.c_str(), color * diffuse);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".specular");                            /* shaderUName = "PointLight.specular" */
  shader->SetVec3f(_uniformName.c_str(), color * specular);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".linear");                              /* shaderUName = "PointLight.linear" */
  shader->SetFloat(_uniformName.c_str(), attenuation.linear);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".quadratic");                           /* shaderUName = "PointLight.quadratic" */
  shader->SetFloat(_uniformName.c_str(), attenuation.quadratic);

  _uniformName.erase(uniformNameSize);                         /* shaderUName = "PointLight" */
}

String PointLight::ToString() const
{
  OStringStream oss;
  oss << "type=PointLight" << "\n";
  oss << "tag=" << tagName << "\n";
  oss << "color=" << color.x << "," << color.y << "," << color.z << "\n";
  oss << "ambient=" << ambient << "\n";
  oss << "diffuse=" << diffuse << "\n";
  oss << "specular=" << specular << "\n";
  oss << "position=" << position.x << "," << position.y << "," << position.z << "\n";
  return oss.str();
}