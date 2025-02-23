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
	 * @brief Creates a StaticMesh from a file located at the given path and allocates GPU resources.
	 *
	 * This method imports a 3D model from the specified file using the Assimp library. It processes the
	 * model by applying several Assimp post-processing steps, such as triangulation, UV coordinate generation,
	 * tangent space calculation, and others. It handles multiple meshes within the model.
	 *
	 * The method allocates memory on the GPU for the meshes by creating Vertex Array Objects (VAO) and
	 * attaching buffers for vertex and index data. For each mesh, it loads the vertex and index data into
	 * separate buffers (VBOs for vertices and EBO for indices), and links them to the VAO for rendering.
	 *
	 * - Allocates VAO for each mesh.
	 * - Creates and attaches Vertex Buffer Objects (VBOs) for vertex data.
	 * - Creates and attaches Element Buffer Object (EBO) for index data.
	 * - Allocates GPU resources only once per mesh, preventing multiple allocations for identical meshes.
	 *
	 * Additionally, if the model has associated textures (e.g., diffuse, specular, normal maps), these
	 * textures are loaded and assigned to the corresponding materials.
	 *
	 * @param absolute The absolute path to the 3D model file to load.
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

	/** @brief Unique identifier referencing the original StaticMesh stored in ModelsManager. */
	u32 id;

private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	Buffer LoadVertices(aiMesh* aimesh);
	Buffer LoadIndices(aiMesh* aimesh);
};