#include "SpotLight.hh"

namespace Lighting
{
  SpotLight::SpotLight(string uniformName) : PointLight(uniformName)
  {
    direction = vec3f(0.0f, -1.0f, 0.0f);   // default from top to bottom
    cutOff    = 12.0f;
  }

  void SpotLight::Render(Graphics::Shader* shader)
  {
    const int UNameSize = uniformName.size();
    const int rsize = 30 - UNameSize;
    char shaderUName[30] = { };

    strcpy_s(shaderUName, 30, uniformName.c_str());           // shaderUName = "SpotLight"
    strcpy_s((shaderUName + UNameSize), rsize, ".position");         // shaderUName = "SpotLight.position"
    shader->SetVec3f(shaderUName, position);

    strcpy_s((shaderUName + UNameSize), rsize, ".direction"); // shaderUName = "SpotLight.direction"
    shader->SetVec3f(shaderUName, direction);

    strcpy_s((shaderUName + UNameSize), rsize, ".ambient");   // shaderUName = "SpotLight.ambient"
    shader->SetVec3f(shaderUName, color * ambient);

    strcpy_s((shaderUName + UNameSize), rsize, ".diffuse");   // shaderUName = "SpotLight.diffuse"
    shader->SetVec3f(shaderUName, color * diffuse);

    strcpy_s((shaderUName + UNameSize), rsize, ".specular");  // shaderUName = "SpotLight.specular"
    shader->SetVec3f(shaderUName, color * specular);

    strcpy_s((shaderUName + UNameSize), rsize, ".linear");    // shaderUName = "SpotLight.linear"
    shader->SetFloat(shaderUName, attenuation.linear);

    strcpy_s((shaderUName + UNameSize), rsize, ".quadratic"); // shaderUName = "SpotLight.quadratic"
    shader->SetFloat(shaderUName, attenuation.quadratic);

    strcpy_s((shaderUName + UNameSize), rsize, ".cutOff");     // shaderUName = "SpotLight.cutOff"
    shader->SetFloat(shaderUName, glm::cos(glm::radians(cutOff)));
  }
}