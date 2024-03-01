#pragma once

#include "Core/Core.hpp"
#include "Engine/Scene.hpp"
#include "Core/Log/Logger.hpp"
#include <entt/entt.hpp> /* Entity component system */

enum class GameObjectType : uint32_t {
	NONE = 0,
	STATIC_MESH,
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};


/* ------------------------------------------------------------------
	Base object class represents a generic object placed in world.
	------------------------------------------------------------------- */
class GameObject
{
public:
	GameObject(entt::entity entity, uint32_t type, class Scene* scene)
		: _entity{ entity },
			_scene{ scene },
			_type{ type }
	{}

	/* Add new component to object */
	template<class T, class...Args>
	T& AddComponent(Args&&... args)
	{
		if (GetComponent<T>())
			CONSOLE_WARN("Object already has component!");
		
		return _scene->Reg().emplace<T>(_entity, std::forward<Args>(args)...);
	}

	/* Return the pointer to component T from object. It can return nullptr! */
	template<class T>
	T* GetComponent()
	{
		return _scene->Reg().try_get<T>(_entity);
	}

	/* Remove component T from object */
	template<class T>
	void RemoveComponent()
	{
		if (!GetComponent<T>())
		{
			CONSOLE_WARN("Object does not have component!");
			return;
		}
		_scene->Reg().remove<T>(_entity);
	}

	/* Return object id */
	entt::entity GetEntity() const { return _entity; }

	/* Compare two objects by IDs */
	bool Compare(const GameObject& other) const { return _entity == other.GetEntity(); }

	/* Return object type */
	uint32_t GetType() const { return _type; }

private:
	entt::entity _entity; /* entt::entity -> uint32_t */
	uint32_t _type;
	class Scene* _scene;
};
