#pragma once

#include "Core/Types.hpp"
#include "Core/Strings.hpp"
#include "Core/FileSystem.hpp"
#include "Engine/ECS/Types.hpp"

class Scene;
class Entity;

namespace YAML
{
  class Emitter;
  class Node;

  namespace detail
  {
    struct iterator_value;
  };
};

class SceneSerializer
{
public:
  SceneSerializer(Scene& scene) : _scene{ &scene }{}

  void Save(const fs::path& absolutePath);
  void Load(const fs::path& absolutePath);

private:
  void __SerializeArchetypeStaticMesh(YAML::Emitter& outEmitter, Entity entity);
  void __SerializeArchetypeLightSource(YAML::Emitter& outEmitter, Entity entity);
  void __SerializeArchetypeCamera(YAML::Emitter& outEmitter, Entity entity);

  void __DeserializeArchetypeStaticMesh(const YAML::detail::iterator_value& yamlEntityNode,
                                        ArchetypeId archetypeId,
                                        StringView tag);
  void __DeserializeArchetypeLightSource(const YAML::detail::iterator_value& yamlEntityNode,
                                         ArchetypeId archetypeId,
                                         StringView tag);
  void __DeserializeArchetypeCamera(const YAML::detail::iterator_value& yamlEntityNode,
                                    ArchetypeId archetypeId, 
                                    StringView tag);
  
  Scene* _scene;
};