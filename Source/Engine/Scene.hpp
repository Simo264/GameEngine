#pragma once

#include "Core/Core.hpp"
#include <entt/entt.hpp> /* Entity component system */

/**
 * Scenes are where you work with content. They are assets that contain all or part of a game or application. 
 * For example, you might build a simple game in a single scene, while for a more complex game, 
 * you might use one scene per level, each with its own environments, characters, obstacles, decorations, and UI.
 */
class Scene
{
public:
	Scene() = default;
	Scene(StringView filePath);
	~Scene() = default;

	void LoadScene(StringView filePath);

	void SaveScene(StringView filePath);

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

	void SerializeScene(StringView filePath);
	void DeserializeScene(StringView filePath);
};
