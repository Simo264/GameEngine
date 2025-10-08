#pragma once

#include "Core/Strings.hpp"
#include "Core/FileSystem.hpp"

#include "EntityBuilder.hpp"
#include "Engine/ECS/Components/Transform.hpp"

class Scene;

class StaticMeshEntityBuilder : public EntityBuilder
{
public:
	StaticMeshEntityBuilder(Scene& scene) : 
		_scene{ &scene },
		_archetypeId{ INVALID_ARCHETYPE_ID },
		_tag{},
		_transform{},
		_absoluteModelPath{}
	{}

	void WithArchetypeIdentifier(ArchetypeId archetype)
	{
		_archetypeId = archetype;
	}
	void WithTag(StringView tag)
	{
		_tag = String(tag.data());
	}
	void WithTransform(const Components::Transform& transform)
	{
		_transform = transform;
	}
	void WithModelPath(const fs::path& absolutePath)
	{
		_absoluteModelPath = absolutePath;
	}

	Entity Build();

private:
	Scene* _scene;
	ArchetypeId _archetypeId;
	String _tag;
	Components::Transform _transform;
	fs::path _absoluteModelPath;
};
