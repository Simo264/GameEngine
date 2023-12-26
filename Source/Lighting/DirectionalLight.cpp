#include "DirectionalLight.hh"


DirectionalLight::DirectionalLight(string uniformName) : BaseLight()
{
  this->uniformName = uniformName;
  direction = vec3f(0.0f, -1.0f, 0.0f);   // default from top to bottom
}

void DirectionalLight::Render(Shader* shader)
{
  const int UNameSize = uniformName.size();
  const int rsize = 30 - UNameSize;
  char shaderUName[30] = { };
    
  strcpy_s(shaderUName, 30, uniformName.c_str());             // shaderUName = "DirLight"
  strcpy_s((shaderUName + UNameSize), rsize, ".direction");   // shaderUName = "DirLight.direction"
  shader->SetVec3f(shaderUName, direction);
    
  strcpy_s((shaderUName + UNameSize), rsize, ".ambient");     // shaderUName = "DirLight.ambient"
  shader->SetVec3f(shaderUName, color * ambient);
    
  strcpy_s((shaderUName + UNameSize), rsize, ".diffuse");     // shaderUName = "DirLight.diffuse"
  shader->SetVec3f(shaderUName, color * diffuse);
    
  strcpy_s((shaderUName + UNameSize), rsize, ".specular");    // shaderUName = "DirLight.specular"
  shader->SetVec3f(shaderUName, color * specular);
}
