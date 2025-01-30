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
	StaticMesh() = default;
	/** @brief Default destructor. */
	~StaticMesh() = default;

	void CreateFromPath(const fs::path& path);
	void Destroy();

	void Draw(RenderMode mode);

	/** @brief Returns the total number of vertices in the mesh. */
	u32 TotalVertices() const;
	
	/** @brief Returns the total number of indices in the mesh. */
	u32 TotalIndices() const;

	/** @brief List of meshes contained in the model. */
	Vector<Mesh> meshes;

	fs::path path; // relative path to "Assets/Models/Static"

private:
	/** @brief Processes a node in the model's scene graph. */
	void ProcessNode(aiNode* node, const aiScene* scene);
	/**
	 * @brief Loads vertex data from an Assimp mesh..
	 * @return Buffer containing vertex data.
	 */
	Buffer LoadVertices(aiMesh* aimesh);
	/**
	 * @brief Loads index data from an Assimp mesh.
	 * @return Buffer containing index data.
	 */
	Buffer LoadIndices(aiMesh* aimesh);
	/** @brief Retrieves the appropriate texture from a material. */
	const Texture2D* GetMaterialTexture(aiMaterial* material, u32 textureType);
};