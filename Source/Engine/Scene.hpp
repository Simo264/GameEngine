#pragma once

#include "Core/Core.hpp"
#include <entt/entt.hpp> /* Entity component system */

/**
 * Contains all objects that are placed in world.
 */
class Scene
{
public:
	Scene() = default;

	void LoadScene(const fspath& filepath);

	void SaveScene(const fspath& filepath);

	/**
	 * Remove all objects from scene
	 */
	void ClearScene();

	/**
	 * Create object in scene
	 */
	class GameObject CreateObject();

	/**
	 * Destroy an entity and releases its identifier
	 */
	void DestroyObject(class GameObject& object);

	/**
	 * Return the reference of registry
	 */
	entt::registry& Reg() { return _registry; }

private:
	/* We can create a entt::registry to store our entities */
	entt::registry _registry;
};
