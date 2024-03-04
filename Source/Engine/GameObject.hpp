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
	GameObject()
		: _entity{ 0xFFFFFFFF }, /* Invalid entity */
			_reg{ nullptr } 
	{}

	GameObject(entt::entity id, entt::registry* reg)
		: _entity{ (uint32_t)id },
			_reg{ reg }
	{}

	template<class T, class...Args>
	T& AddComponent(Args&&... args)
	{
		if (GetComponent<T>())
			CONSOLE_WARN("Object already has component!");
		
		return _reg->emplace_or_replace<T>(_entity, std::forward<Args>(args)...);
	}

	template<class T>
	T* GetComponent()
	{
		return _reg->try_get<T>(_entity);
	}

	template<class T>
	void RemoveComponent()
	{
		if (!GetComponent<T>())
			CONSOLE_WARN("Object does not have component!");
		
		_reg->remove<T>(_entity);
	}

	entt::entity GetObjectID() const { return _entity; }

	bool IsValid() const {  return (_reg && (uint32_t)_entity != 0xFFFFFFFF); }

private:
	entt::entity		_entity;	/* Object id */
	entt::registry* _reg;			/* Pointer to scene registry */
};
