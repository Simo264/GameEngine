#include "Scene.hpp"
#include "SceneSerializer.hpp"

#include "Core/Log/Logger.hpp"

// -----------------------------------
//					ObjectArchetype
// -----------------------------------

void Archetype::GetAllowedComponentNames(Vector<String>& out) const
{
	out = Vector<String>{};
	for (const auto& typeIndex : _allowedComponents)
		out.push_back(typeIndex.name());
}
bool Archetype::IsComponentAllowed(std::type_index componentType) const
{
	return _allowedComponents.count(componentType) > 0;
}

// -----------------------------------
//					ArchetypeRegistry
// -----------------------------------

ArchetypeId ArchetypeRegistry::RegisterArchetype(Archetype archetype)
{
	auto name = archetype.GetName();
	auto id = _archetypes.size();

	_archetypes.emplace_back(std::move(archetype));
	_nameToIdMap[name] = id;
	return id;
}
const Archetype* ArchetypeRegistry::GetArchetype(ArchetypeId id) const
{
	if (id >= _archetypes.size())
		return nullptr;
	
	return &_archetypes[id];
}
const Archetype* ArchetypeRegistry::GetArchetype(StringView archName) const
{
	auto id = GetArchetypeId(archName.data());
	return GetArchetype(id);
}
ArchetypeId ArchetypeRegistry::GetArchetypeId(StringView name) const
{
	auto it = _nameToIdMap.find(name.data());
	if (it != _nameToIdMap.end())
		return it->second;
	
	return INVALID_ARCHETYPE_ID;
}
void ArchetypeRegistry::GetAvailableArchetypeNames(Vector<String>& out) const
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
void Scene::DestroyObject(entt::entity id)
{
	if (_entityRegistry.valid(id))
		_entityRegistry.destroy(id);
}
void Scene::Clear()
{
	_entityRegistry.clear();
}

void Scene::RegisterArchetype(Archetype archetype)
{
	_archetypeRegistry.RegisterArchetype(std::move(archetype));
}
GameObject Scene::CreateObject(StringView archetypeName)
{
	auto archetype = _archetypeRegistry.GetArchetype(archetypeName);
	if (!archetype)
	{
		CONSOLE_ERROR("Unknown archetype: {}", archetypeName);
		throw std::runtime_error("Unknown archetype");
	}
	
	auto archetypeId = _archetypeRegistry.GetArchetypeId(archetype->GetName());
	auto entity = _entityRegistry.create();
	
	auto object = GameObject{ entity, &_entityRegistry, archetype };
	object.AddComponent<ArchetypeComponent>(archetypeId);
	auto& tag = object.AddComponent<Components::Tag>();
	tag.UpdateValue("object");
	return object;
}
GameObject Scene::CreateObject(ArchetypeId archetypeId)
{
	auto archetype = _archetypeRegistry.GetArchetype(archetypeId);
	if (!archetype)
	{
		CONSOLE_ERROR("Invalid archetype ID: {}", archetypeId);
		throw std::runtime_error("Invalid archetype ID");
	}
	
	auto entity = _entityRegistry.create();
	auto object = GameObject{ entity, &_entityRegistry, archetype };
	object.AddComponent<ArchetypeComponent>(archetypeId);
	auto& tag = object.AddComponent<Components::Tag>();
	tag.UpdateValue("object");
	return object;
}

Optional<GameObject> Scene::FindObjectWithTag(StringView tagName)
{
	auto view = _entityRegistry.view<Components::Tag>();
	for (auto entity : view)
	{
		auto& archetypeComponent = _entityRegistry.get<ArchetypeComponent>(entity);
		auto archetype = _archetypeRegistry.GetArchetype(archetypeComponent.archetypeId);

		auto& tag = _entityRegistry.get<Components::Tag>(entity);
		if (tagName.compare(tag.value.data()) == 0)
			return GameObject(entity, &_entityRegistry, archetype);
	}
	return std::nullopt;
}
void Scene::FindAllWithArchetype(ArchetypeId archetypeId, Vector<GameObject>& out)
{
	auto archetype = _archetypeRegistry.GetArchetype(archetypeId);
	if (!archetype)
	{
		CONSOLE_ERROR("Invalid archetype id {}", archetypeId);
		return;
	}

	out = Vector<GameObject>{};
	auto view = _entityRegistry.view<ArchetypeComponent>();
	for (auto entity : view)
	{
		auto& archetypeComp = _entityRegistry.get<ArchetypeComponent>(entity);
		auto archetypeId = archetypeComp.archetypeId;
		if (archetypeComp.archetypeId == archetypeId)
			out.emplace_back(entity, &_entityRegistry, archetype);
	}
}
void Scene::FindAllWithArchetype(StringView archetypeName, Vector<GameObject>& out)
{
	auto archetype = _archetypeRegistry.GetArchetype(archetypeName);
	if (!archetype)
	{
		CONSOLE_ERROR("Invalid archetype name {}", archetypeName);
		return;
	}

	out = Vector<GameObject>{};
	auto view = _entityRegistry.view<ArchetypeComponent>();
	for (auto entity : view)
	{
		const auto& archetypeComp = _entityRegistry.get<ArchetypeComponent>(entity);
		auto archetypeId = archetypeComp.archetypeId;
		if (archetypeName.compare(archetype->GetName()) == 0)
			out.emplace_back(entity, &_entityRegistry, archetype);
	}
}
