#include "Material.hpp"

#include "Engine/Uniforms.hpp"

void Material::Bind() const
{
  diffuse.BindTextureUnit(UniformBindings::TEXTURE_COLOR);
  specular.BindTextureUnit(UniformBindings::TEXTURE_SPECULAR);
  normal.BindTextureUnit(UniformBindings::TEXTURE_NORMAL);
}
