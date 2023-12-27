#include "SpotLight.hh"

SpotLight::SpotLight(const char* uniformName) : PointLight(uniformName)
{
  direction = vec3f(0.0f, -1.0f, 0.0f);   // default from top to bottom
  cutOff    = 12.0f;
}

void SpotLight::Render(Shader* shader)
{
  const int uniformNameSize = uniformName.size();

  uniformName.append(".position");
  shader->SetVec3f(uniformName.c_str(), position);

  uniformName.erase(uniformNameSize);
  uniformName.append(".direction");
  shader->SetVec3f(uniformName.c_str(), direction);

  uniformName.erase(uniformNameSize);
  uniformName.append(".ambient");
  shader->SetVec3f(uniformName.c_str(), color * ambient);
  
  uniformName.erase(uniformNameSize);
  uniformName.append(".diffuse");
  shader->SetVec3f(uniformName.c_str(), color * diffuse);

  uniformName.erase(uniformNameSize);
  uniformName.append(".specular");
  shader->SetVec3f(uniformName.c_str(), color * specular);

  uniformName.erase(uniformNameSize);
  uniformName.append(".linear");
  shader->SetFloat(uniformName.c_str(), attenuation.linear);

  uniformName.erase(uniformNameSize);
  uniformName.append(".quadratic");
  shader->SetFloat(uniformName.c_str(), attenuation.quadratic);

  uniformName.erase(uniformNameSize);
  uniformName.append(".cutOff");
  shader->SetFloat(uniformName.c_str(), glm::cos(glm::radians(cutOff)));

  uniformName.erase(uniformNameSize);
}
