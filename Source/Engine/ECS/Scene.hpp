#pragma once

#include "Core/Functional.hpp"
#include "Entity.hpp"
#include "Archetype.hpp"
#include "Components/Components.hpp"

using namespace Components;

class Scene
{
public:
	Scene(): 
		_entityRegistry{},
		_archetypeRegistry{}
	{}
	~Scene() { Clear(); }

	void InitializeDefaultArchetypes();

	/** @brief unregisters the given entity in the registry and removes all its components. */
	void DestroyEntity(EntityId id);
	/** @brief Destroys all entities in the scene and clears the registry. */
	void Clear();

	void RegisterArchetype(const Archetype& archetype);

	Entity CreateEntity(ArchetypeId archetypeId, StringView tag = "entity");

	template<typename Component, typename... Args>
	Component& AssignEntityComponent(Entity entity, Args&&... args)
	{
		auto& archIdComp = GetEntityComponent<Components::ArchetypeIdentifier>(entity);
		auto& archetype = _archetypeRegistry.GetArchetype(archIdComp.archetypeId);
		
		constexpr auto compTypeId = entt::type_hash<Component>::value();
		if (!archetype.IsComponentAllowed(compTypeId))
			throw std::runtime_error("Component not allowed for this archetype");

		return _entityRegistry.emplace<Component>(entity.Id(), std::forward<Args>(args)...);
	}

	template<typename Component>
	void RemoveEntityComponent(Entity entity)
	{
		_entityRegistry.remove<Component>(entity.Id());
	}

	template<typename Component>
	Component& GetEntityComponent(Entity entity)
	{
		return _entityRegistry.get<Component>(entity.Id());
	}

	template<typename Component>
	Component* TryGetEntityComponent(Entity entity)
	{
		return _entityRegistry.try_get<Component>(entity.Id());
	}

	template<typename Component>
	Optional<Entity> FindEntityWithComponent()
	{
		auto view = _entityRegistry.view<Component>();
		if (view.empty())
			return std::nullopt;
		
		auto entityId = *view.begin();
		return Entity{ entityId };
	}

	template<typename Component>
	void FindAllEntitiesWithComponent(Vector<Entity>& out)
	{
		out.clear();
		auto view = _entityRegistry.view<Component>();
		out.reserve(view.size());
		for (auto entity : view)
			out.emplace_back(entity);
	}

	Optional<Entity> FindEntityWithTag(StringView tagName);

	auto& GetEntityRegistry() { return _entityRegistry; }
	auto& GetArchetypeRegistry() { return _archetypeRegistry; }

private:
	EntityRegistry _entityRegistry;
	ArchetypeRegistry _archetypeRegistry;
};
