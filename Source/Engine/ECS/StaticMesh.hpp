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
	/** @brief Default constructor. */
	StaticMesh() : 
		meshes{ nullptr },
		nrMeshes{ 0 },
		id{ 0 }
	{}
	/** @brief Default destructor. */
	~StaticMesh() = default;

	/**
	 * @brief Creates a static mesh from the specified file path.
	 * This method loads and initializes a static mesh object using the given absolute path.
	 * It is responsible for reading mesh data from the file and preparing it for rendering.
	 *
	 * @param absolute The absolute path to the static mesh file.
	 */
	void CreateFromPath(const fs::path& absolute);
	
	void Clone(StaticMesh& other) const;

	void Destroy();

	void Draw(RenderMode mode) const;

	/** @brief Returns the total number of vertices in the mesh. */
	u32 TotalVertices() const;
	
	/** @brief Returns the total number of indices in the mesh. */
	u32 TotalIndices() const;

	/** @brief List of meshes contained in the model. */
	Mesh* meshes;
	u32 nrMeshes;

	u32 id;

private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	Buffer LoadVertices(aiMesh* aimesh);
	Buffer LoadIndices(aiMesh* aimesh);
};