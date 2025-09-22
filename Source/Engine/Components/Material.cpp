#include "Material.hpp"
#include "Engine/Managers/TexturesManager.hpp"

namespace Components
{
  Material::Material()
  {
    const auto& fallback = TexturesManager::GetInstance().GetFallbackTexture();
    albedo = fallback;
    normalMap = fallback;
  }
}

