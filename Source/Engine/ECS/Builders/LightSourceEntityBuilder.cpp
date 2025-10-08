#include "LightSourceEntityBuilder.hpp"
#include "Engine/ECS/Scene.hpp"

using namespace Components;

Entity LightSourceEntityBuilder::Build()
{
	if (_scene == nullptr || _archetypeId == INVALID_ARCHETYPE_ID)
		throw std::runtime_error("Builder not initialized correctly (missing Scene or ArchetypeId).");

	auto entity = _scene->CreateEntity(_archetypeId);
	auto& tagComponent = _scene->GetEntityComponent<Tag>(entity);
	tagComponent.Update(_tag.c_str());

	if(_lightType == LightType::None)
		throw std::runtime_error("Invalid light type");
  
  _scene->AssignEntityComponent<Light>(entity);
  switch (_lightType)
  {
    case LightType::Directional:
      _scene->AssignEntityComponent<DirectionalLight>(entity);
      break;
    case LightType::Point:
      _scene->AssignEntityComponent<PointLight>(entity);
      break;
    case LightType::Spot:
      _scene->AssignEntityComponent<SpotLight>(entity);
      break;
    default:
      throw std::runtime_error("Unsupported light type");
  }

  return entity;
}
