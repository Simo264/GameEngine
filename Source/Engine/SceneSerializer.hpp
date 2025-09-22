#pragma once

#include "Core/Core.hpp"
#include "Core/YAMLParser.hpp"
#include "Components/Components.hpp"

class Scene;
class Entity;

using namespace Components;

class SceneSerializer
{
public:
  SceneSerializer() = default;
  void SerializeScene(Scene& scene, const fs::path& outPath);
  void DeserializeScene(Scene& scene, const fs::path& fromPath);

private:
  void __SerializeTag(YAML::Emitter& out, const Tag& tag);
  void __SerializeTransformation(YAML::Emitter& out, const Transform& transform);
  void __SerializeStaticMesh(YAML::Emitter& out, const StaticMesh& staticMesh, AssetIdentifier assetIdentifier);
  void __SerializeLight(YAML::Emitter& out, const Light& light, Entity& entity);
  void __SerializeDirectionalLight(YAML::Emitter& out, const DirectionalLight& light);
  void __SerializePointLight(YAML::Emitter& out, const PointLight& light);
  void __SerializeSpotLight(YAML::Emitter& out, const SpotLight& light);

  void __DeserializeTag(Entity& entity, const YAML::Node& node);
  void __DeserializeTransformation(Entity& entity, const YAML::Node& node);
  void __DeserializeStaticMesh(Entity& entity, const YAML::Node& node);
  void __DeserializeLight(Entity& entity, const YAML::Node& node);
  void __DeserializeDirLight(Entity& entity, const YAML::Node& node);
  void __DeserializePointLight(Entity& entity, const YAML::Node& node);
  void __DeserializeSpotLight(Entity& entity, const YAML::Node& node);
};