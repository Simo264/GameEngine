#include "CameraEntityBuilder.hpp"
#include "Engine/ECS/Scene.hpp"

using namespace Components;

Entity CameraEntityBuilder::Build()
{
	if (_scene == nullptr || _archetypeId == INVALID_ARCHETYPE_ID)
		throw std::runtime_error("Builder not initialized correctly (missing Scene or ArchetypeId).");

	auto entity = _scene->CreateEntity(_archetypeId);
	auto& tagComponent = _scene->GetEntityComponent<Tag>(entity);
	tagComponent.Update(_tag.c_str());
	
	_scene->AssignEntityComponent<Camera>(entity);
	return entity;
}
