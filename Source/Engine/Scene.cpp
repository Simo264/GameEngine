#include "Scene.hpp"
#include "SceneSerializer.hpp"

#include "Core/Logger.hpp"


// -----------------------------------
//					EntityArchetype
// -----------------------------------

void EntityArchetype::GetAllowedComponentNames(Vector<String>& out) const
{
	out = Vector<String>{};
	for (const auto& typeIndex : _allowedComponents)
		out.push_back(typeIndex.name());
}
bool EntityArchetype::IsComponentAllowed(std::type_index componentType) const
{
	return _allowedComponents.count(componentType) > 0;
}

// -----------------------------------
//					ArchetypeRegistry
// -----------------------------------

ArchetypeId EntityArchetypeRegistry::RegisterArchetype(EntityArchetype archetype)
{
	auto name = archetype.GetName();
	auto id = _archetypes.size();

	//_archetypes.emplace_back(std::move(archetype));
	_archetypes.push_back(archetype);
	_nameToIdMap[name] = id;
	return id;
}
const EntityArchetype* EntityArchetypeRegistry::GetArchetype(ArchetypeId id) const
{
	if (id >= _archetypes.size())
		return nullptr;
	
	return &_archetypes[id];
}
ArchetypeId EntityArchetypeRegistry::GetArchetypeId(StringView name) const
{
	auto it = _nameToIdMap.find(name.data());
	if (it != _nameToIdMap.end())
		return it->second;
	
	return INVALID_ARCHETYPE_ID;
}
void EntityArchetypeRegistry::GetArchetypeNames(Vector<String>& out) const
{
	out = Vector<String>{};
	out.reserve(_archetypes.size());
	for (const auto& archetype : _archetypes)
		out.push_back(archetype.GetName());
}

// -----------------------------------
//					Scene
// -----------------------------------

void Scene::LoadFromFile(const fs::path& loadFrom)
{
	CONSOLE_INFO("Loading scene {}...", loadFrom.relative_path().string());
	auto serializer = SceneSerializer{};
	serializer.DeserializeScene(*this, loadFrom);
}
void Scene::SaveToFile(const fs::path& out)
{
	CONSOLE_INFO("Saving scene {}...", out.relative_path().string());
	auto serializer = SceneSerializer{};
	serializer.SerializeScene(*this, out);
}
void Scene::DestroyEntity(EntityId id)
{
	if (_entityRegistry.valid(id))
		_entityRegistry.destroy(id);
}
void Scene::Clear()
{
	Vector<u32> bufferIds;
	Vector<u32> vertexArrayIds;

	auto view = _entityRegistry.view<StaticMesh>();
	for (auto entity : view)
	{
		auto& mesh = view.get<StaticMesh>(entity);
		if (mesh.vertexArray.IsValid())
		{
			vertexArrayIds.push_back(mesh.vertexArray.id);
			mesh.vertexArray.id = INVALID_VERTEXARRAY_ID;
		}
		if (mesh.vertexBuffer.IsValid())
		{
			bufferIds.push_back(mesh.vertexBuffer.id);
			mesh.vertexBuffer.id = INVALID_BUFFER_ID;
		}
		if (mesh.indexBuffer.IsValid())
		{
			bufferIds.push_back(mesh.indexBuffer.id);
			mesh.indexBuffer.id = INVALID_BUFFER_ID;
		}
	}

	// Batch delete
	if (!vertexArrayIds.empty())
		VertexArray::DeleteArrays(vertexArrayIds.size(), vertexArrayIds.data());
	if (!bufferIds.empty())
		Buffer::DeleteBuffers(bufferIds.size(), bufferIds.data());

	_entityRegistry.clear();
}

void Scene::RegisterArchetype(EntityArchetype archetype)
{
	_archetypeRegistry.RegisterArchetype(archetype);
}
Entity Scene::CreateEntity(StringView archetypeName)
{
	auto archetypeId = _archetypeRegistry.GetArchetypeId(archetypeName);
	auto archetype = _archetypeRegistry.GetArchetype(archetypeId);
	if (!archetype)
	{
		CONSOLE_ERROR("Unknown archetype: {}", archetypeName);
		throw std::runtime_error("Unknown archetype");
	}

	auto e = _entityRegistry.create();
	auto entity = Entity{ e, &_entityRegistry, archetype };
	entity.AddComponent<Components::ArchetypeIdentifier>(archetypeId);
	auto& tag = entity.AddComponent<Components::Tag>();
	tag.UpdateValue("entity");
	return entity;
}
Entity Scene::CreateEntity(ArchetypeId archetypeId)
{
	auto archetype = _archetypeRegistry.GetArchetype(archetypeId);
	if (!archetype)
	{
		CONSOLE_ERROR("Invalid archetype ID: {}", archetypeId);
		throw std::runtime_error("Invalid archetype ID");
	}
	
	auto e = _entityRegistry.create();
	auto entity = Entity{ e, &_entityRegistry, archetype };
	entity.AddComponent<Components::ArchetypeIdentifier>(archetypeId);
	auto& tag = entity.AddComponent<Components::Tag>();
	tag.UpdateValue("entity");
	return entity;
}

Optional<Entity> Scene::FindEntityWithTag(StringView tagName)
{
	auto view = _entityRegistry.view<EntityId>();
	for (auto entity : view)
	{
		auto& archetypeComponent = _entityRegistry.get<Components::ArchetypeIdentifier>(entity);
		auto archetype = _archetypeRegistry.GetArchetype(archetypeComponent.archetypeId);

		auto& tag = _entityRegistry.get<Components::Tag>(entity);
		if (tagName.compare(tag.value.data()) == 0)
			return Entity(entity, &_entityRegistry, archetype);
	}
	return std::nullopt;
}
void Scene::FindAllWithArchetype(ArchetypeId archetypeId, Vector<Entity>& out)
{
	auto archetype = _archetypeRegistry.GetArchetype(archetypeId);
	if (!archetype)
	{
		CONSOLE_ERROR("Invalid archetype id {}", archetypeId);
		return;
	}

	out = Vector<Entity>{};
	auto view = _entityRegistry.view<EntityId>();
	for (auto entity : view)
	{
		auto& archetypeComponent = _entityRegistry.get<Components::ArchetypeIdentifier>(entity);
		if (archetypeComponent.archetypeId == archetypeId)
			out.emplace_back(entity, &_entityRegistry, archetype);
	}
}

