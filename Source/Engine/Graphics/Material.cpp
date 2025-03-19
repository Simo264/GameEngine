#include "Material.hpp"

#include "Engine/Graphics/Shader.hpp"

void Material::Bind(Program program) const
{
  program.SetUniform1i("u_material.diffuseTexture", 0);
  program.SetUniform1i("u_material.specularTexture", 1);
  program.SetUniform1i("u_material.normalTexture", 2);

  diffuse.BindTextureUnit(0);
  specular.BindTextureUnit(1);
  normal.BindTextureUnit(2);
}
