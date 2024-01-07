#include "PointLight.hh"
#include "../Graphics/Shader.hh"

void PointLight::RenderLight(Shader* shader)
{
  const uint64_t uniformNameSize = _uniformName.size();
  
  _uniformName.append(".position");                            // shaderUName = "PointLight.position"
  shader->SetVec3f(_uniformName.c_str(), position);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".ambient");                             // shaderUName = "PointLight.ambient"
  shader->SetVec3f(_uniformName.c_str(), color * ambient);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".diffuse");                             // shaderUName = "PointLight.diffuse"
  shader->SetVec3f(_uniformName.c_str(), color * diffuse);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".specular");                            // shaderUName = "PointLight.specular"
  shader->SetVec3f(_uniformName.c_str(), color * specular);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".linear");                              // shaderUName = "PointLight.linear"
  shader->SetFloat(_uniformName.c_str(), attenuation.linear);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".quadratic");                           // shaderUName = "PointLight.quadratic"
  shader->SetFloat(_uniformName.c_str(), attenuation.quadratic);

  _uniformName.erase(uniformNameSize);                         // shaderUName = "PointLight"
}
