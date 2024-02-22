#include "DirectionalLight.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Logger.hpp"

void DirectionalLight::RenderLight(Shader* shader)
{
  const uint64_t uniformNameSize = _uniformName.size();

  /* uniformName = "DirLight.direction" */ 
  _uniformName.append(".direction");                     
  shader->SetVec3f(_uniformName.c_str(), direction);
    
  /* uniformName = "DirLight.ambient" */ 
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".ambient"); 
  shader->SetVec3f(_uniformName.c_str(), color * ambient);
    
  /* uniformName = "DirLight.diffuse" */ 
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".diffuse");    
  shader->SetVec3f(_uniformName.c_str(), color * diffuse);
    
  /* uniformName = "DirLight.specular" */ 
  _uniformName.erase(uniformNameSize);
  _uniformName.append(".specular");
  shader->SetVec3f(_uniformName.c_str(), color * specular);

  /* uniformName = "DirLight" */   
  _uniformName.erase(uniformNameSize);
}