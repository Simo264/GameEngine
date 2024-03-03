#pragma once

#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"
#include <entt/entt.hpp> /* Entity component system */

/* ------------------------------------------------------------------
	Base object class represents a generic object placed in world.
	------------------------------------------------------------------- */
class GameObject
{
public:
	GameObject(entt::entity entity, entt::registry* reg)
		: _entity{ entity },
			_reg{ reg }
	{}

	/* Add new component to object */
	template<class T, class...Args>
	T& AddComponent(Args&&... args)
	{
		if (GetComponent<T>())
			CONSOLE_WARN("Object already has component!");
		
		return _reg->emplace_or_replace<T>(_entity, std::forward<Args>(args)...);
	}

	/* Return the pointer to component T from object. It can return nullptr! */
	template<class T>
	T* GetComponent()
	{
		return _reg->try_get<T>(_entity);
	}

	/* Remove component T from object */
	template<class T>
	void RemoveComponent()
	{
		if (!GetComponent<T>())
			CONSOLE_WARN("Object does not have component!");
		
		_reg->remove<T>(_entity);
	}

	/* Return object id */
	entt::entity GetEntity() const { return _entity; }

	/* Compare two objects by IDs */
	bool Compare(const GameObject& other) const { return _entity == other.GetEntity(); }

private:
	entt::entity		_entity;	/* Object id */
	entt::registry* _reg;			/* Pointer to scene registry */
};
