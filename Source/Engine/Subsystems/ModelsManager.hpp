#pragma once

#include "Core/Core.hpp"
#include "Engine/ECS/StaticMesh.hpp"
#include "Engine/ECS/Skeleton/SkeletalMesh.hpp"

/**
 * @class ModelsManager
 * @brief Manages the loading, storage, and retrieval of static and skeletal meshes.
 *
 * This singleton class ensures that each unique static or skeletal mesh is loaded only once from a file.
 * Subsequent requests for the same mesh return a reference to the already loaded instance, avoiding redundant
 * file reads and GPU memory allocations. Copies of meshes maintain separate CPU-side data but share GPU resources.
 *
 * @note The class follows the singleton pattern, preventing direct instantiation and enforcing centralized management.
 */
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
		* If the static mesh does not exist, it is loaded, allocated in GPU memory, and stored for future use.
		* All instances of the same static mesh share the same GPU buffers, avoiding redundant memory allocation.
		* However, CPU-side data is duplicated when cloning.
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
		* If the skeletal mesh does not exist, it is loaded, allocated in GPU memory, and stored for future use.
		* Cloned instances share GPU resources but duplicate CPU-side data.
		*
		* @param relative The relative path to the skeletal mesh inside "Models/Skeletal".
		* Example: "Character/Character.gltf".
		*
		* @return Reference to the newly created SkeletalMesh.
		*/
	const SkeletalMesh& CreateSkeletalMesh(const fs::path& relative);

	/**
	 * @brief Retrieves the file path of a static mesh based on its unique ID.
	 *
	 * @param staticMeshId The unique identifier of the static mesh.
	 * @return Pointer to the file path if found, nullptr otherwise.
	 */
	const fs::path* GetStaticMeshPath(u32 staticMeshId) const;

	/**
	 * @brief Retrieves the file path of a skeletal mesh based on its unique ID.
	 *
	 * @param skeletalMeshId The unique identifier of the skeletal mesh.
	 * @return Pointer to the file path if found, nullptr otherwise.
	 */
	const fs::path* GetSkeletalMeshPath(u32 skeletalMeshId) const;

private:
	ModelsManager() = default;
	~ModelsManager() = default;

	// Stores all loaded static mesh objects.
	// Static meshes are only loaded once, and their GPU resources are shared among all instances. 
	Vector<StaticMesh> _staticMeshObjects;
	// Each index in this vector corresponds to the same index in `_staticMeshObjects`,
	// ensuring that `_staticMeshObjects[i]` represents the mesh loaded from `_staticMeshPaths[i]`.
	Vector<fs::path> _staticMeshPaths;

	// Stores all loaded skeletal mesh objects.
	// Skeletal meshes are only loaded once, and their GPU resources are shared among all instances.
	Vector<SkeletalMesh> _skeletalMeshObjects;
	// Each index in this vector corresponds to the same index in `_skeletalMeshObjects`,
	// ensuring that `_skeletalMeshObjects[i]` represents the mesh loaded from `_skeletalMeshPaths[i]`.
	Vector<fs::path> _skeletalMeshPaths;
};
