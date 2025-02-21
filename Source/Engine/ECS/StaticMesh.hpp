#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Mesh.hpp"

class Texture2D;
class Buffer;
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;


/**
 * @class StaticMesh
 * @brief Handles the loading, processing, and rendering of static 3D models.
 * 
 * The StaticMesh class is responsible for loading a 3D model from a file,
 * processing its nodes and meshes, and rendering it. It also manages vertex
 * and index buffers and retrieves textures from materials.
 */
class StaticMesh
{
public:
	StaticMesh();

	/**
	 * @brief Releases CPU-side memory used by the static mesh.
	 *
	 * This destructor frees the allocated memory for the `meshes` array. It does not
	 * affect GPU resources, as those must be explicitly released using `Destroy()`
	 * before deleting the object.
	 *
	 * @note The destructor is called when a `StaticMesh` component is removed from an
	 *       object or when the original instance is destroyed by `ModelsManager`.
	 */
	~StaticMesh() = default;

	/** @brief Move constructor */
	StaticMesh(StaticMesh&&) noexcept = default;
	StaticMesh& operator=(StaticMesh&&) noexcept = default;

	/** @brief Delete copy constructor */
	StaticMesh(const StaticMesh&) = delete;
	StaticMesh& operator=(const StaticMesh&) = delete;

	/**
	 * @brief Creates a static mesh from the specified file path.
	 * This method loads and initializes a static mesh object using the given absolute path.
	 * It is responsible for reading mesh data from the file and preparing it for rendering.
	 *
	 * @param absolute The absolute path to the static mesh file.
	 */
	void CreateFromPath(const fs::path& absolute);
	
	/**
	 * @brief Creates a copy of the current static mesh with shared VAOs but unique materials.
	 *
	 * This method allocates new memory for the `Mesh` array in `other`,
	 * ensuring that each copy has its own independent materials.
	 * However, the VAOs (vertex array objects) remain shared between all copies.
	 *
	 * @note Since VAOs are shared, changes to the mesh geometry will affect all copies.
	 *       However, materials can be modified independently.
	 */
	void Clone(StaticMesh& other) const;

	/**
	 * @brief Frees GPU memory by destroying OpenGL buffers and VAOs.
	 *
	 * This method releases all OpenGL-related resources, including vertex array objects (VAOs)
	 * and associated buffers. It should only be called on the original instance to avoid
	 * affecting shared meshes.
	 *
	 * @note Since VAOs and buffers are shared across copies, calling `Destroy()` on a copy
	 *       would result in invalid references. Ensure that only the original instance calls this method.
	 */
	void Destroy() const;

	void Draw(RenderMode mode) const;

	u32 TotalVertices() const;
	
	u32 TotalIndices() const;

	/** @brief List of meshes contained in the model. */
	UniquePtr<Mesh[]> meshes;
	u32 nrMeshes;

	u32 id;

private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	Buffer LoadVertices(aiMesh* aimesh);
	Buffer LoadIndices(aiMesh* aimesh);
};