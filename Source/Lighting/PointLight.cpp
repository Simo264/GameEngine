#include "PointLight.hh"


namespace Lighting
{
  PointLight::PointLight(string uniformName) : BaseLight()
  {
    this->uniformName = uniformName;
    position = vec3f(0.0f, 0.0f, 0.0f);  // default on origin
  }

  void PointLight::Render(Graphics::Shader* shader)
  {
    const int UNameSize = uniformName.size();
    const int rsize = 30 - UNameSize;
    char shaderUName[30] = { };
    
    strcpy_s(shaderUName, 30, uniformName.c_str());             // shaderUName = "PointLight"
    strcpy_s((shaderUName + UNameSize), rsize, ".position");    // shaderUName = "PointLight.position"
    shader->SetVec3f(shaderUName, position);
    
    strcpy_s((shaderUName + UNameSize), rsize, ".ambient");     // shaderUName = "PointLight.ambient"
    shader->SetVec3f(shaderUName, color * ambient);
    
    strcpy_s((shaderUName + UNameSize), rsize, ".diffuse");     // shaderUName = "PointLight.diffuse"
    shader->SetVec3f(shaderUName, color * diffuse);
    
    strcpy_s((shaderUName + UNameSize), rsize, ".specular");    // shaderUName = "PointLight.specular"
    shader->SetVec3f(shaderUName, color * specular);
    
    strcpy_s((shaderUName + UNameSize), rsize, ".linear");      // shaderUName = "PointLight.linear"
    shader->SetFloat(shaderUName, attenuation.linear);
    
    strcpy_s((shaderUName + UNameSize), rsize, ".quadratic");   // shaderUName = "PointLight.quadratic"
    shader->SetFloat(shaderUName, attenuation.quadratic);
  }
}