#pragma once

#include "Core/Core.hpp"
#include "Components/Components.hpp"
#include <entt/entt.hpp>

using EntityId = entt::entity;
using EntityRegistry = entt::registry;
constexpr EntityId INVALID_ENTITY_ID = entt::null;

class EntityArchetype
{
public:
	EntityArchetype(StringView name) : _archetypeName{ name.data() } {}

	template<typename... Comps>
	auto& AllowComponents()
	{
		_allowedComponents = { std::type_index(typeid(Comps))... };
		_allowedComponents.insert(std::type_index(typeid(Components::ArchetypeIdentifier)));
		_allowedComponents.insert(std::type_index(typeid(Components::Tag)));
		return *this;
	}

	bool IsComponentAllowed(std::type_index componentType) const;
	void GetAllowedComponentNames(Vector<String>& out) const;
	auto GetName() const { return _archetypeName.data(); }

private:
	String _archetypeName;
	Set<std::type_index> _allowedComponents;
};
class EntityArchetypeRegistry
{
public:
	ArchetypeId RegisterArchetype(EntityArchetype archetype);
	ArchetypeId GetArchetypeId(StringView name) const;
	const EntityArchetype* GetArchetype(ArchetypeId id) const;
	void GetArchetypeNames(Vector<String>& out) const;
	auto GetArchetypeCount() const { return _archetypes.size(); }
	auto& GetArchetypeVector() const { return _archetypes; }

private:
	Vector<EntityArchetype> _archetypes;
	UnorderedMap<String, ArchetypeId> _nameToIdMap;
};
class Entity
{
public:
	Entity(EntityId id, EntityRegistry* entityReg, const EntityArchetype* archetype) :
		_id{ id }, 
		_entityRegistry{ entityReg },
		_archetype{ archetype }
	{}

	template<typename Component, typename... Args>
	auto& AddComponent(Args&&... args)
	{
		if(!_archetype)
			throw std::runtime_error("Invalid archetype pointer");

		auto componentType = std::type_index(typeid(Component));
		if (!_archetype->IsComponentAllowed(componentType))
			throw std::runtime_error("Component not allowed for this archetype");

		return _entityRegistry->emplace<Component>(_id, std::forward<Args>(args)...);
	}

	template <typename T>
	T* GetComponent()
	{
		return _entityRegistry->try_get<T>(_id);
	}

	template<typename Component>
	void RemoveComponent()
	{
		_entityRegistry->remove<Component>(_id);
	}

	template <typename T>
	bool HasComponent()
	{
		return _entityRegistry->any_of<T>(_id);
	}

	auto GetID() const { return _id; }
	
	auto GetArchetype() const { return _archetype; }
	
	auto IsValid() const
	{
		return
			_entityRegistry != nullptr &&
			_archetype != nullptr &&
			_entityRegistry->valid(_id);
	}
	auto Compare(const Entity& other) const { return _id == other.GetID(); }

private:
	EntityId _id;
	const EntityArchetype* _archetype;
	EntityRegistry* _entityRegistry;
};
class Scene
{
public:
	Scene() : _entityRegistry{}, _archetypeRegistry{} {}

	/** @brief Loads a scene from a file. */
	void LoadFromFile(const fs::path& loadFrom);
	/** @brief Saves the current scene into a file. */
	void SaveToFile(const fs::path& out);
	/** @brief unregisters the given entity in the registry and removes all its components. */
	void DestroyEntity(EntityId id);
	/** @brief Destroys all entities in the scene and clears the registry. */
	void Clear();

	void RegisterArchetype(EntityArchetype archetype);
	Entity CreateEntity(StringView archetypeName);
	Entity CreateEntity(ArchetypeId archetypeId);

	template<typename Component>
	Optional<Entity> FindEntityWithComponent()
	{
		auto view = _entityRegistry.view<Component>();
		if (view.empty())
			return std::nullopt;

		auto entity = *view.begin();
		auto& archetypeComponent = _entityRegistry.get<Components::ArchetypeIdentifier>(entity);
		auto archetypeId = archetypeComponent.archetypeId;
		auto archetype = _archetypeRegistry.GetArchetype(archetypeId);
		return Entity{ entity, &_entityRegistry, archetype };
	}

	template<typename Component>
	void FindAllEntitiesWithComponent(Vector<Entity>& out)
	{
		out = Vector<Entity>{};
		auto view = _entityRegistry.view<Component>();
		for (auto entity : view)
		{
			auto& archetypeComponent = _entityRegistry.get<Components::ArchetypeIdentifier>(entity);
			auto archetypeId = archetypeComponent.archetypeId;
			auto archetype = _archetypeRegistry.GetArchetype(archetypeId);
			out.emplace_back(entity, &_entityRegistry, archetype);
		}
	}

	Optional<Entity> FindEntityWithTag(StringView tagName);
	void FindAllWithArchetype(ArchetypeId archetypeId, Vector<Entity>& out);

	auto& GetEntityRegistry() { return _entityRegistry; }
	auto& GetArchetypeRegistry() { return _archetypeRegistry; }

private:
	EntityRegistry _entityRegistry;
	EntityArchetypeRegistry _archetypeRegistry;
};
