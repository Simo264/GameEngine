#include "Material.hpp"

#include "Engine/Uniforms.hpp"

void Material::Bind() const
{
  albedo.BindTextureUnit(10);
  normalMap.BindTextureUnit(11);
}
