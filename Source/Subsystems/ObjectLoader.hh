#pragma once

#include "../Core.hh"
#include "../Texture2D.hh"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Read data from .obj files inside Assets directory
// -------------------------------------------------
class ObjectLoader
{
public:
	ObjectLoader() = delete;
	~ObjectLoader() = delete;
	
	// Load data for single mesh
	static void LoadSingleMeshData(
		std::filesystem::path filePath, 
		vector<float>&				vertDest, 
		vector<uint32_t>&			indDest,
		array<std::filesystem::path, 2>& texturePaths);

	// Load data for multiple meshes
	// static void LoadMultipleMeshData();

	static std::filesystem::path assetsDirPath;

private:
	static bool CheckScene(const aiScene* scene);

	static void LoadVertices(const aiMesh* aimesh, vector<float>& vertDest);
	static void LoadIndices(const aiMesh* aimesh, vector<uint32_t>& indDest);
	static std::filesystem::path GetTexturePath(const struct aiMaterial* material, const char* textureType);
};