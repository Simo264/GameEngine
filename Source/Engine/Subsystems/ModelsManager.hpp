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
	 * @return Pointer to the newly created StaticMesh.
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
	 * @return Pointer to the newly created SkeletalMesh.
	 */
	const SkeletalMesh* CreateSkeletalMesh(const fs::path& relative);

	const fs::path* GetStaticMeshPath(u32 staticMeshId) const;

	const fs::path* GetSkeletalMeshPath(u32 skeletalMeshId) const;

	void DestroyAll();

private:
	ModelsManager() = default;
	~ModelsManager() = default;

	Vector<StaticMesh> _staticMeshObjects;
	// Map storing the static mesh index and it's relative path
	Vector<std::pair<u32, fs::path>> _staticMeshPaths;

	// Skeletal mesh id, skeletal mesh object
	Map<u32, SkeletalMesh> _skeletalMeshObjects;
	// Skeletal mesh id, skeletal mesh (relative) path
	Vector<std::pair<u32, fs::path>> _skeletalMeshPaths;
};
