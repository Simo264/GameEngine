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
	/** @brief Default constructor for Scene. */
	Scene() = default;
	/** @brief Constructs a Scene by loading from a file. */
	Scene(const fs::path& filePath);
	/** @brief Default destructor for Scene. */
	~Scene() = default;

	/** @brief Loads a scene from a file. */
	void LoadFromFile(const fs::path& filePath);
	/** @brief Saves the current scene to a file. */
	void Save(const fs::path& filePath);
	/** @brief Clears all objects and data from the current scene. */
	void Clear();

	/**
	 * @brief Creates a new object in the scene.
	 * 
	 * @param objName Optional name for the object.
	 * 
	 * @return The created GameObject.
	 */
	GameObject CreateObject(StringView objName = "");
	
	/** @brief Destroys an object in the scene. */
	void DestroyObject(GameObject& object);

	/** @brief Provides access to the internal entity registry. */
	entt::registry& Reg() { return _registry; }

private:
	// We can create a entt::registry to store our entities
	entt::registry _registry;

	void SerializeScene(const fs::path& filePath);
	void DeserializeScene(const fs::path& filePath);
};
