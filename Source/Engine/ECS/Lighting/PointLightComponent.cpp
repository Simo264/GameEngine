#include "PointLightComponent.hpp"
#include "Engine/Graphics/Shader.hpp"

void PointLightComponent::Render(Shader* shader)
{
  const uint64_t uniformNameSize = uniform.size();

  /* shaderUName = "PointLight.position" */
  uniform.append(".position");                            
  shader->SetVec3f(uniform.c_str(), position);

  /* shaderUName = "PointLight.ambient" */
  uniform.erase(uniformNameSize);
  uniform.append(".ambient");         
  shader->SetVec3f(uniform.c_str(), color * ambient);

  /* shaderUName = "PointLight.diffuse" */
  uniform.erase(uniformNameSize);
  uniform.append(".diffuse");                             
  shader->SetVec3f(uniform.c_str(), color * diffuse);

  /* shaderUName = "PointLight.specular" */
  uniform.erase(uniformNameSize);
  uniform.append(".specular");   
  shader->SetVec3f(uniform.c_str(), color * specular);

  /* shaderUName = "PointLight.linear" */
  uniform.erase(uniformNameSize);
  uniform.append(".linear");                              
  shader->SetFloat(uniform.c_str(), linear);

  /* shaderUName = "PointLight.quadratic" */
  uniform.erase(uniformNameSize);
  uniform.append(".quadratic");
  shader->SetFloat(uniform.c_str(), quadratic);

  /* shaderUName = "PointLight" */
  uniform.erase(uniformNameSize);                         
}
