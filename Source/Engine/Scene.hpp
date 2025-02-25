#pragma once

#include "Core/Core.hpp"
#include <entt/entt.hpp> // Entity component system

class GameObject;

/**
 * @class Scene
 * 
 * @brief Represents and manages a 3D scene, including loading, saving, and manipulating objects.
 */
class Scene
{
public:
	Scene() = default;
	Scene(const fs::path& filePath);
	~Scene() = default;

	/** @brief Loads a scene from a file. */
	void LoadFromFile(const fs::path& filePath);
	/** @brief Saves the current scene to a file. */
	void Save(const fs::path& filePath);
	
	/**
	 * @brief Destroys all objects in the scene and clears the registry.
	 *
	 * This method removes all entities from the registry and calls the destructors
	 * of all associated components. After this operation, the scene will be empty.
	 *
	 * @note This does not free the memory used by component pools, but it resets them.
	 */
	void Clear();

	GameObject CreateObject(StringView objName = "object");
	
	/**
	 * @brief Destroys a specific game object and removes all its components.
	 *
	 * This method unregisters the given object from the scene by destroying its associated
	 * entity in the registry. The destructors of all its components will be called automatically.
	 *
	 * @param object The game object to be destroyed.
	 */
	void DestroyObject(entt::entity id);

	/** @brief Provides access to the internal entity registry. */
	entt::registry& Reg() { return _registry; }

private:
	// We can create a entt::registry to store our entities
	entt::registry _registry;

	void SerializeScene(const fs::path& filePath);
	void DeserializeScene(const fs::path& filePath);
};
