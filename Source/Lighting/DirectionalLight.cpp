#include "DirectionalLight.hh"

namespace Lighting
{
  DirectionalLight::DirectionalLight(string uniformName) : BaseLight(), uniformName{ uniformName }
  {
    direction = vec3f(0.0f, -1.0f, 0.0f);   // default from top to bottom
  }

  void DirectionalLight::Render(Graphics::Shader* shader)
  {
    const int UNameSize = uniformName.size();
    char shaderUName[30] = { };
    
    strcpy_s(shaderUName, 30, uniformName.c_str());   // shaderUName = "dirLight"
    strcpy_s((shaderUName + UNameSize), (30 - UNameSize), ".direction"); // shaderUName = "dirLight.direction"
    shader->SetVec3f(shaderUName, direction);
    strcpy_s((shaderUName + UNameSize), (30 - UNameSize), ".ambient");     // shaderUName = "dirLight.ambient"
    shader->SetVec3f(shaderUName, color * ambient);
    strcpy_s((shaderUName + UNameSize), (30 - UNameSize), ".diffuse");     // shaderUName = "dirLight.diffuse"
    shader->SetVec3f(shaderUName, color * diffuse);
    strcpy_s((shaderUName + UNameSize), (30 - UNameSize), ".specular");    // shaderUName = "dirLight.specular"
    shader->SetVec3f(shaderUName, color * specular);
  }
}