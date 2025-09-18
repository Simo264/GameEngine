#pragma once

#include "Core/Core.hpp"
#include "Components/Components.hpp"

#include <entt/entt.hpp>

using EntityId = entt::entity;
using EntityRegistry = entt::registry;
using ArchetypeId = u32;

constexpr EntityId INVALID_ENTITY_ID = entt::null;
constexpr ArchetypeId INVALID_ARCHETYPE_ID = static_cast<ArchetypeId>(-1);

struct ArchetypeComponent
{
	ArchetypeId archetypeId;
	ArchetypeComponent(ArchetypeId id) : archetypeId(id) {}
};

class Archetype
{
public:
	Archetype(StringView name) : _archetypeName{ name.data() } {}

	template<typename... Components>
	auto& AllowComponents()
	{
		_allowedComponents = { std::type_index(typeid(Components))... };
		return *this;
	}

	bool IsComponentAllowed(std::type_index componentType) const;
	void GetAllowedComponentNames(Vector<String>& out) const;
	auto GetName() const { return _archetypeName.data(); }

private:
	String _archetypeName;
	UnorderedSet<std::type_index> _allowedComponents;
};
class ArchetypeRegistry
{
public:
	ArchetypeId RegisterArchetype(Archetype archetype);
	
	const Archetype* GetArchetype(ArchetypeId id) const;
	const Archetype* GetArchetype(StringView archName) const;
	
	ArchetypeId GetArchetypeId(StringView name) const;
	
	void GetAvailableArchetypeNames(Vector<String>& out) const;

	auto GetArchetypeCount() const {return _archetypes.size(); }

private:
	Vector<Archetype> _archetypes;
	UnorderedMap<String, ArchetypeId> _nameToIdMap;
};
class GameObject
{
public:
	GameObject(EntityId id, EntityRegistry* entityReg, const Archetype* archetype) :
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
	auto Compare(const GameObject& other) const { return _id == other.GetID(); }

private:
	EntityId _id;
	const Archetype* _archetype;
	EntityRegistry* _entityRegistry;
};
class Scene
{
public:
	Scene() : _entityRegistry{}, _archetypeRegistry{} {}
	~Scene() = default;

	/** @brief Loads a scene from a file. */
	void LoadFromFile(const fs::path& loadFrom);
	/** @brief Saves the current scene into a file. */
	void SaveToFile(const fs::path& out);
	/** @brief unregisters the given entity in the registry and removes all its components. */
	void DestroyObject(entt::entity id);
	/** @brief Destroys all objects in the scene and clears the registry. */
	void Clear();

	void RegisterArchetype(Archetype archetype);
	GameObject CreateObject(StringView archetypeName);
	GameObject CreateObject(ArchetypeId archetypeId);

	template<typename Component>
	Optional<GameObject> FindObjectWithComponent()
	{
		auto view = _entityRegistry.view<Component>();
		if (view.empty())
			return std::nullopt;

		auto entity = *view.begin();
		auto& archetypeComponent = _entityRegistry.get<ArchetypeComponent>(entity);
		auto archetypeId = archetypeComponent.archetypeId;
		auto archetype = _archetypeRegistry.GetArchetype(archetypeId);
		return GameObject(entity, &_entityRegistry, archetype);
	}

	template<typename Component>
	void FindAllObjectsWithComponent(Vector<GameObject>& out)
	{
		out = Vector<GameObject>{};
		auto view = _entityRegistry.view<Component>();
		for (auto entity : view)
		{
			auto& archetypeComponent = _entityRegistry.get<ArchetypeComponent>(entity);
			auto archetypeId = archetypeComponent.archetypeId;
			auto archetype = _archetypeRegistry.GetArchetype(archetypeId);
			out.emplace_back(entity, &_entityRegistry, archetype);
		}
	}

	Optional<GameObject> FindObjectWithTag(StringView tagName);
	void FindAllWithArchetype(ArchetypeId archetypeId, Vector<GameObject>& out);
	void FindAllWithArchetype(StringView archetypeName, Vector<GameObject>& out);

	auto& GetEntityRegistry() { return _entityRegistry; }
	auto& GetArchetypeRegistry() { return _archetypeRegistry; }

private:
	EntityRegistry _entityRegistry;
	ArchetypeRegistry _archetypeRegistry;
};
