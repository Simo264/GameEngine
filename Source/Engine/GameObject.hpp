#pragma once

#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"
#include <entt/entt.hpp> 

/**
 * Represents the entity in Entity Component System
 */
class GameObject
{
public:
	GameObject()
		: _entity{ entt::null },
			_reg{ nullptr } 
	{}

	GameObject(entt::entity id, entt::registry* reg)
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

	entt::entity GetObjectID() const 
	{ 
		return _entity; 
	}

	void Invalidate()
	{
		_entity = entt::null;
		_reg = nullptr;
	}

	bool IsEqual(const GameObject& rhs) const 
	{
		return static_cast<uint32_t>(_entity) == static_cast<uint32_t>(rhs.GetObjectID());
	}

	operator bool() const 
	{ 
		return (_reg && _entity != entt::null); 
	}

private:
	entt::entity _entity;	/* Object id */
	entt::registry* _reg;	/* Pointer to scene registry */
};
