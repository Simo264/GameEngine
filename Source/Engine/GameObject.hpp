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

	bool Compare(const GameObject& other) const
	{
		return static_cast<uint32_t>(_entity) == static_cast<uint32_t>(other.GetObjectID());
	}

	entt::entity GetObjectID() const 
	{ 
		return _entity; 
	}

	bool IsValid() const 
	{  
		return (_reg && _entity != entt::null); 
	}

	void Invalidate() 
	{ 
		_entity = entt::null;
		_reg = nullptr;
	}

private:
	entt::entity		_entity;	/* Object id */
	entt::registry* _reg;			/* Pointer to scene registry */
};
