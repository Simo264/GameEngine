#include "DirectionalLight.hh"
#include "../Shader.hh"

void DirectionalLight::RenderLight(Shader* shader)
{
  const uint64_t uniformNameSize = _uniformName.size();

  _uniformName.append(".direction");                     // uniformName = "DirLight.direction"
  shader->SetVec3f(_uniformName.c_str(), direction);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".ambient");                       // uniformName = "DirLight.ambient"
  shader->SetVec3f(_uniformName.c_str(), color * ambient);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".diffuse");                       // uniformName = "DirLight.diffuse"
  shader->SetVec3f(_uniformName.c_str(), color * diffuse);
    
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".specular");                      // uniformName = "DirLight.specular"
  shader->SetVec3f(_uniformName.c_str(), color * specular);

  _uniformName.erase(uniformNameSize);                   // uniformName = "DirLight"  
}
