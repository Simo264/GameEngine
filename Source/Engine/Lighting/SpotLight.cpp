#include "SpotLight.hpp"
#include "Engine/Shader.hpp"

#if 0
void SpotLight::RenderLight(Shader* shader)
{
  const int uniformNameSize = _uniformName.size();

  _uniformName.append(".position");
  shader->SetVec3f(_uniformName.c_str(), position);

  _uniformName.erase(uniformNameSize);
  _uniformName.append(".direction");
  shader->SetVec3f(_uniformName.c_str(), direction);

  _uniformName.erase(uniformNameSize);
  _uniformName.append(".ambient");
  shader->SetVec3f(_uniformName.c_str(), color * ambient);
  
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".diffuse");
  shader->SetVec3f(_uniformName.c_str(), color * diffuse);

  _uniformName.erase(uniformNameSize);
  _uniformName.append(".specular");
  shader->SetVec3f(_uniformName.c_str(), color * specular);

  _uniformName.erase(uniformNameSize);
  _uniformName.append(".linear");
  shader->SetFloat(_uniformName.c_str(), attenuation.linear);

  _uniformName.erase(uniformNameSize);
  _uniformName.append(".quadratic");
  shader->SetFloat(_uniformName.c_str(), attenuation.quadratic);

  _uniformName.erase(uniformNameSize);
  _uniformName.append(".cutOff");
  shader->SetFloat(_uniformName.c_str(), glm::cos(glm::radians(cutOff)));

  _uniformName.erase(uniformNameSize);
}
#endif