#include "DirectionalLight.hpp"

#include "Engine/Shader.hpp"
#include "Engine/Logger.hpp"

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

String DirectionalLight::ToString() const
{
  OStringStream oss;
  oss << "type=DirectionalLight" << "\n";
  oss << "tag=" << tagName << "\n";
  oss << "color=" << color.x << "," << color.y << "," << color.z << "\n";
  oss << "ambient=" << ambient << "\n";
  oss << "diffuse=" << diffuse << "\n";
  oss << "specular=" << specular << "\n";
  oss << "direction=" << direction.x << "," << direction.y << "," << direction.z << "\n";
  return oss.str();
}