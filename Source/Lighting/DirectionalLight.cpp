#include "DirectionalLight.hh"
#include "../Graphics/Shader.hh"

void DirectionalLight::Render(Shader* shader)
{
  const uint64_t uniformNameSize = uniformName.size();

  uniformName.append(".direction");                     // uniformName = "DirLight.direction"
  shader->SetVec3f(uniformName.c_str(), direction);
    
  uniformName.erase(uniformNameSize);
  uniformName.append(".ambient");                       // uniformName = "DirLight.ambient"
  shader->SetVec3f(uniformName.c_str(), color * ambient);
    
  uniformName.erase(uniformNameSize);
  uniformName.append(".diffuse");                       // uniformName = "DirLight.diffuse"
  shader->SetVec3f(uniformName.c_str(), color * diffuse);
    
  uniformName.erase(uniformNameSize);
  uniformName.append(".specular");                      // uniformName = "DirLight.specular"
  shader->SetVec3f(uniformName.c_str(), color * specular);

  uniformName.erase(uniformNameSize);                   // uniformName = "DirLight"  
}
