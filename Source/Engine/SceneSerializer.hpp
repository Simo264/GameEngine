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
  void __SerializeTag(YAML::Emitter& outEmitter, const Tag& tag) const;
  void __SerializeCamera(YAML::Emitter& outEmitter, const Camera& camera) const;
  void __SerializeTransform(YAML::Emitter& outEmitter, const Transform& transform) const;
  void __SerializeStaticMesh(YAML::Emitter& outEmitter, const StaticMesh& staticMesh, AssetIdentifier assetIdentifier) const;
  void __SerializeLight(YAML::Emitter& outEmitter, const Light& light, Entity& entity) const;
  void __SerializeDirectionalLight(YAML::Emitter& outEmitter, const DirectionalLight& light) const;
  void __SerializePointLight(YAML::Emitter& outEmitter, const PointLight& light) const;
  void __SerializeSpotLight(YAML::Emitter& outEmitter, const SpotLight& light) const;

  void __DeserializeTag(Entity& entity, const YAML::Node& node) const;
  void __DeserializeCamera(Entity& entity, const YAML::Node& node) const;
  void __DeserializeTransform(Entity& entity, const YAML::Node& node) const;
  void __DeserializeStaticMesh(Entity& entity, const YAML::Node& node) const;
  void __DeserializeLight(Entity& entity, const YAML::Node& node) const;
  void __DeserializeDirLight(Entity& entity, const YAML::Node& node) const;
  void __DeserializePointLight(Entity& entity, const YAML::Node& node) const;
  void __DeserializeSpotLight(Entity& entity, const YAML::Node& node) const;
};