#include "PointLight.hh"

PointLight::PointLight(const char* uniformName) : BaseLight()
{
  this->uniformName.reserve(50); // Pre-allocation memory
  this->uniformName.append(uniformName);

  this->uniformName = uniformName;
  position = vec3f(0.0f, 0.0f, 0.0f);  // default on origin
}

void PointLight::Render(Shader* shader)
{
  const int uniformNameSize = uniformName.size();
  
  uniformName.append(".position");                            // shaderUName = "PointLight.position"
  shader->SetVec3f(uniformName.c_str(), position);
    
  uniformName.erase(uniformNameSize);
  uniformName.append(".ambient");                             // shaderUName = "PointLight.ambient"
  shader->SetVec3f(uniformName.c_str(), color * ambient);
    
  uniformName.erase(uniformNameSize);
  uniformName.append(".diffuse");                             // shaderUName = "PointLight.diffuse"
  shader->SetVec3f(uniformName.c_str(), color * diffuse);
    
  uniformName.erase(uniformNameSize);
  uniformName.append(".specular");                            // shaderUName = "PointLight.specular"
  shader->SetVec3f(uniformName.c_str(), color * specular);
    
  uniformName.erase(uniformNameSize);
  uniformName.append(".linear");                              // shaderUName = "PointLight.linear"
  shader->SetFloat(uniformName.c_str(), attenuation.linear);
    
  uniformName.erase(uniformNameSize);
  uniformName.append(".quadratic");                           // shaderUName = "PointLight.quadratic"
  shader->SetFloat(uniformName.c_str(), attenuation.quadratic);

  uniformName.erase(uniformNameSize);                         // shaderUName = "PointLight"
}
