#pragma once

#include "Core/Containers.hpp"
#include "Core/Singleton.hpp"
#include "Engine/ECS/Types.hpp"
#include "Engine/ECS/Components/Material.hpp"
#include "Engine/ECS/Components/StaticMesh.hpp"

struct PrototypeData
{
  Components::StaticMesh mesh{};
  Components::Material material{};
};

class StaticMeshPrototypeManager : public Singleton<StaticMeshPrototypeManager>
{
public:
  PrototypeData& LoadPrototype(AssetId assetId);
  PrototypeData* GetPrototype(AssetId assetId);
  
  void Clear() { _prototypes.clear(); }

private:
  Map<AssetId, PrototypeData> _prototypes;
};
