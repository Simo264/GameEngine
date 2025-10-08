#include "StaticMeshEntityBuilder.hpp"
#include "Engine/ECS/Scene.hpp"
#include "Engine/Importers/StaticMeshLoader.hpp"
#include "Engine/Managers/AssetsManager.hpp"
#include "Engine/Managers/StaticMeshPrototypeManager.hpp"

using namespace Components;

Entity StaticMeshEntityBuilder::Build()
{
  if (_scene == nullptr || _archetypeId == INVALID_ARCHETYPE_ID)
    throw std::runtime_error("Builder not initialized correctly (missing Scene or ArchetypeId).");

  auto entity = _scene->CreateEntity(_archetypeId);

  auto& tagComponent = _scene->GetEntityComponent<Tag>(entity);
  tagComponent.Update(_tag.c_str());

  auto& transformComponent = _scene->AssignEntityComponent<Transform>(entity, _transform);
  auto& materialComponent = _scene->AssignEntityComponent<Material>(entity);
  auto& meshComponent = _scene->AssignEntityComponent<StaticMesh>(entity);
  meshComponent.Create();
  using Vertex = StaticMesh::Vertex;
  Vertex::SetupVertexArray(meshComponent.vertexArray);

  auto& assetsManager = AssetsManager::GetInstance();
  auto& prototypeManager = StaticMeshPrototypeManager::GetInstance();
  
  if(_absoluteModelPath.empty())
    throw std::runtime_error("Invalid model path");

  auto assetId = assetsManager.GetAssetId(_absoluteModelPath);
  if (assetId == INVALID_ASSET_ID)
  {
    // Register the asset and load the prototype
    assetId = assetsManager.RegisterAsset(_absoluteModelPath);
    auto& prototypeData = prototypeManager.LoadPrototype(assetId);
    prototypeData.mesh.Create();

    // Load data from file
    auto loader = StaticMeshLoader{};
    loader.LoadDataFromFile(_absoluteModelPath, prototypeData.mesh, prototypeData.material);
  }
  _scene->AssignEntityComponent<AssetIdentifier>(entity, assetId);

  auto prototypeData = prototypeManager.GetPrototype(assetId);
  prototypeData->mesh.Copy(meshComponent);
  materialComponent = prototypeData->material;
  return entity;
}
