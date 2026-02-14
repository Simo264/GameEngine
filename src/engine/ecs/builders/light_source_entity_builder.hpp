#pragma once

#include "Core/Strings.hpp"
#include "EntityBuilder.hpp"
#include "Engine/ECS/Components/Light.hpp"

class Scene;

class LightSourceEntityBuilder : public EntityBuilder
{
public:
	LightSourceEntityBuilder(Scene& scene) :
		_scene{ &scene },
		_archetypeId{ INVALID_ARCHETYPE_ID },
		_tag{},
		_lightType{ LightType::None }
	{}

	void WithArchetypeIdentifier(ArchetypeId archetype)
	{
		_archetypeId = archetype;
	}
	void WithTag(StringView tag)
	{
		_tag = String(tag.data());
	}
	void WithLightType(LightType type)
	{
		_lightType = type;
	}

	Entity Build();
private:
	Scene* _scene;
	ArchetypeId _archetypeId;
	String _tag;
	LightType _lightType;
};
