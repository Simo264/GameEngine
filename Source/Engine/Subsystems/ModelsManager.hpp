#pragma once

#include "Core/Core.hpp"
#include "Engine/ECS/StaticMesh.hpp"
#include "Engine/ECS/Skeleton/SkeletalMesh.hpp"

class ModelsManager
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	ModelsManager(const ModelsManager&) = delete;
	/** @brief Deleted assignment constructor to enforce singleton pattern. */
	void operator=(const ModelsManager&) = delete;

	/** @brief Retrieves the singleton instance of ModelsManager. */
	static ModelsManager& Get()
	{
		static ModelsManager modelsManager;
		return modelsManager;
	}

	/** @brief Cleans up all loaded models by freeing their allocated resources. */
	void CleanUp();

	/**
	 * @brief Finds an existing static mesh by its relative path.
	 * This method searches for a previously loaded static mesh using the given relative path.
	 *
	 * @param relative The relative path to the static mesh inside "Models/Static".
	 * Example: "Cube/cube.obj".
	 * 
	 * @return Pointer to the found StaticMesh, or nullptr if not found.
	 */
	const StaticMesh* FindStaticMesh(const fs::path& relative) const;

	/**
	 * @brief Creates and loads a new static mesh from the given relative path.
	 * If the static mesh does not exist, it is loaded and stored for future use.
	 *
	 * @param relative The relative path to the static mesh inside "Models/Static".
	 * Example: "Cube/cube.obj".
	 * 
	 * @return Reference to the newly created StaticMesh.
	 */
	const StaticMesh& CreateStaticMesh(const fs::path& relative);

	/**
	 * @brief Finds an existing skeletal mesh by its relative path.
	 * This method searches for a previously loaded skeletal mesh using the given relative path.
	 *
	 * @param relative The relative path to the skeletal mesh inside "Models/Skeletal".
	 * Example: "Character/Character.gltf".
	 * 
	 * @return Pointer to the found SkeletalMesh, or nullptr if not found.
	 */
	const SkeletalMesh* FindSkeletalMesh(const fs::path& relative) const;

	/**
	 * @brief Creates and loads a new skeletal mesh from the given relative path.
	 * If the skeletal mesh does not exist, it is loaded and stored for future use.
	 *
	 * @param relative The relative path to the skeletal mesh inside "Models/Skeletal".
	 * Example: "Character/Character.gltf".
	 * 
	 * @return Reference to the newly created SkeletalMesh.
	 */
	const SkeletalMesh& CreateSkeletalMesh(const fs::path& relative);

	const fs::path* GetStaticMeshPath(u32 staticMeshId) const;
	const fs::path* GetSkeletalMeshPath(u32 skeletalMeshId) const;

private:
	ModelsManager() = default;
	~ModelsManager() = default;

	// Stores all loaded static mesh objects.
	// Each index in this vector corresponds to the same index in `_staticMeshPaths`,
	// ensuring that `_staticMeshObjects[i]` represents the mesh loaded from `_staticMeshPaths[i]`.
	Vector<StaticMesh> _staticMeshObjects;
	Vector<fs::path> _staticMeshPaths;

	// Stores all loaded skeletal mesh objects.
	// Each index in this vector corresponds to the same index in `_skeletalMeshPaths`,
	// ensuring that `_skeletalMeshObjects[i]` represents the mesh loaded from `_skeletalMeshPaths[i]`.
	Vector<SkeletalMesh> _skeletalMeshObjects;
	Vector<fs::path> _skeletalMeshPaths;
};
