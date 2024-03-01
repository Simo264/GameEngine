#include "SpotLightComponent.hpp"
#include "Engine/Graphics/Shader.hpp"

void SpotLightComponent::Render(class Shader* shader)
{
  const int uniformNameSize = uniform.size();

  uniform.append(".position");
  shader->SetVec3f(uniform.c_str(), position);

  uniform.erase(uniformNameSize);
  uniform.append(".direction");
  shader->SetVec3f(uniform.c_str(), direction);

  uniform.erase(uniformNameSize);
  uniform.append(".ambient");
  shader->SetVec3f(uniform.c_str(), color * ambient);

  uniform.erase(uniformNameSize);
  uniform.append(".diffuse");
  shader->SetVec3f(uniform.c_str(), color * diffuse);

  uniform.erase(uniformNameSize);
  uniform.append(".specular");
  shader->SetVec3f(uniform.c_str(), color * specular);

  uniform.erase(uniformNameSize);
  uniform.append(".linear");
  shader->SetFloat(uniform.c_str(), linear);

  uniform.erase(uniformNameSize);
  uniform.append(".quadratic");
  shader->SetFloat(uniform.c_str(), quadratic);

  uniform.erase(uniformNameSize);
  uniform.append(".cutOff");
  shader->SetFloat(uniform.c_str(), glm::cos(glm::radians(cutOff)));

  uniform.erase(uniformNameSize);
}
