#include "Material.hpp"

#include "Engine/Graphics/Shader.hpp"

void Material::Bind(Program program) const
{
  diffuse.BindTextureUnit(0);
  specular.BindTextureUnit(1);
  normal.BindTextureUnit(2);
}
