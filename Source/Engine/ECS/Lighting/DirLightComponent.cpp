#include "DirLightComponent.hpp"
#include "Engine/Graphics/Shader.hpp"

void DirLightComponent::Render(class Shader* shader)
{
  const uint64_t uniformNameSize = uniform.size();

  /* uniformName = "DirLight.direction" */
  uniform.append(".direction");
  shader->SetVec3f(uniform.c_str(), direction);

  /* uniformName = "DirLight.ambient" */
  uniform.erase(uniformNameSize);
  uniform.append(".ambient");
  shader->SetVec3f(uniform.c_str(), color * ambient);

  /* uniformName = "DirLight.diffuse" */
  uniform.erase(uniformNameSize);
  uniform.append(".diffuse");
  shader->SetVec3f(uniform.c_str(), color * diffuse);

  /* uniformName = "DirLight.specular" */
  uniform.erase(uniformNameSize);
  uniform.append(".specular");
  shader->SetVec3f(uniform.c_str(), color * specular);

  /* uniformName = "DirLight" */
  uniform.erase(uniformNameSize);
}