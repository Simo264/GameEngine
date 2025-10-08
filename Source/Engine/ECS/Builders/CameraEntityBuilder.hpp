#pragma once

#include "EntityBuilder.hpp"

class Scene;

class CameraEntityBuilder : public EntityBuilder
{
public:
	CameraEntityBuilder(Scene& scene) : 
		_scene{ &scene },
		_archetypeId{ INVALID_ARCHETYPE_ID },
		_tag{}
	{}

	void WithArchetypeIdentifier(ArchetypeId archetype)
	{
		_archetypeId = archetype;
	}
	void WithTag(StringView tag)
	{
		_tag = String(tag.data());
	}

	Entity Build();

private:
	Scene* _scene;
	ArchetypeId _archetypeId;
	String _tag;
};
