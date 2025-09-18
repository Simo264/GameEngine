#pragma once

#include "Core/Core.hpp"
#include "Core/Serialization/YAMLParser.hpp"
#include "Components/Components.hpp"

class Scene;
class GameObject;

class SceneSerializer
{
public:
  SceneSerializer() = default;
  void SerializeScene(Scene& scene, const fs::path& outPath);
  void DeserializeScene(Scene& scene, const fs::path& fromPath);

private:
  void __SerializeTag(YAML::Emitter& out, const Components::Tag& tag);
  void __SerializeTransformation(YAML::Emitter& out, const Components::Transform& transform);
  void __SerializeStaticMesh(YAML::Emitter& out, const Components::StaticMesh& staticMesh);
  void __SerializeLight(YAML::Emitter& out, const Components::Light& light, GameObject& object);
  void __SerializeDirectionalLight(YAML::Emitter& out, const Components::DirectionalLight& light);
  void __SerializePointLight(YAML::Emitter& out, const Components::PointLight& light);
  void __SerializeSpotLight(YAML::Emitter& out, const Components::SpotLight& light);

  void __DeserializeTag(GameObject& object, const YAML::Node& component);
  void __DeserializeTransformation(GameObject& object, const YAML::Node& component);
  void __DeserializeStaticMesh(GameObject& object, const YAML::Node& component);
  void __DeserializeLight(GameObject& object, const YAML::Node& component);
  void __DeserializeDirLight(GameObject& object, const YAML::Node& component);
  void __DeserializePointLight(GameObject& object, const YAML::Node& component);
  void __DeserializeSpotLight(GameObject& object, const YAML::Node& component);
};