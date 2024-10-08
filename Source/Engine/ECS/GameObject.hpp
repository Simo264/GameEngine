#pragma once

#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"
#include <entt/entt.hpp> 

/**
 * GameObject is an identifier that represents an object in the game world.
 * It does not contain data per se, but acts as a "container" with wich components are associated.
 * The entity has no behaviour or state without components associated with it.
 */
class GameObject
{
public:
	GameObject(entt::entity id = entt::null, entt::registry* reg = nullptr)
		: _entity{ id },
			_reg{ reg }
	{}

	template<typename T, typename...Args>
	T& AddComponent(Args&&... args)
	{
		if (GetComponent<T>())
			CONSOLE_WARN("Object already has component!");
		
		return _reg->emplace_or_replace<T>(_entity, std::forward<Args>(args)...);
	}

	template<typename T>
	T* GetComponent()
	{
		return _reg->try_get<T>(_entity);
	}

	template<typename T>
	void RemoveComponent()
	{
		if (!GetComponent<T>())
			CONSOLE_WARN("Object does not have component!");

		_reg->remove<T>(_entity);
	}

	template<typename T>
	bool HasComponent()
	{
		return _reg->any_of<T>(_entity);
	}

	constexpr entt::entity ID() const 
	{ 
		return _entity; 
	}

	void Invalidate()
	{
		_entity = entt::null;
		_reg = nullptr;
	}

	constexpr bool IsValid() const
	{
		return (_reg && _reg->valid(_entity));
	}

	bool IsEqual(const GameObject& rhs) const
	{
		return static_cast<u32>(_entity) == static_cast<u32>(rhs.ID());
	}

private:
	entt::entity _entity;	/* In Entt an entity is simply an ID generated by the register */
	entt::registry* _reg;	/* The register */
};
