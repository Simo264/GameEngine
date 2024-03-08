#pragma once

#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"
#include <entt/entt.hpp> /* Entity component system */

enum class GameObjectType : int
{
	NONE = 0,
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,

	STATIC_MESH,

	// SKELETAL_MESH,
	// ...
};


/* ------------------------------------------------------------------
	GameObject class represents the enitity in Entity Component System
	------------------------------------------------------------------- */
class GameObject
{
public:
	GameObject()
		: _entity{ entt::null }, /* entt::null = static_cast<uint32_t>(0xFFFFFFFF) */
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

	bool IsValid() const 
	{  
		return (_reg && _entity != entt::null);
	}

	/* Destroys the entity and releases its identifier */
	void Destroy() 
	{ 
		_reg->destroy(_entity); 
		_entity = entt::null;
		_reg = nullptr;
	}

private:
	entt::entity		_entity;	/* Object id */
	entt::registry* _reg;			/* Pointer to scene registry */
};
