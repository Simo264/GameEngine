#pragma once

#include "Core/Core.hpp"
#include <entt/entt.hpp> /* Entity component system */

class GameObject;

/**
 * @brief
 * Scenes are where you work with content. They are assets that contain all or part of a game or application. 
 * For example, you might build a simple game in a single scene, while for a more complex game, 
 * you might use one scene per level, each with its own environments, characters, obstacles, decorations, and UI.
 */
class Scene
{
public:
	Scene() = default;
	Scene(const fs::path& filePath);
	~Scene() = default;

	void LoadScene(const fs::path& filePath);
	void SaveScene(const fs::path& filePath);

	/**
	 * @brief
	 * Remove all objects from scene
	 */
	void ClearScene();

	/**
	 * @brief
	 * Create object in scene
	 */
	GameObject CreateObject(StringView objName = "");

	/**
	 * @brief
	 * Destroy an entity and releases its identifier
	 */
	void DestroyObject(GameObject& object);

	/**
	 * @return
	 * The reference of registry
	 */
	entt::registry& Reg() { return _registry; }

private:
	/* We can create a entt::registry to store our entities */
	entt::registry _registry;

	void SerializeScene(const fs::path& filePath);
	void DeserializeScene(const fs::path& filePath);
};
