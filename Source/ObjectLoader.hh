#pragma once

#include "Core.hh"
#include "Texture2D.hh"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Load mesh data from .obj files inside Assets directory
// -------------------------------------------------
class ObjectLoader
{
public:
	ObjectLoader(Path filePath);
	~ObjectLoader() = default;
	
	// Load Static Mesh from file
	void LoadMesh(
		Vector<float>& vertDest, 
		Vector<uint32_t>& indDest, 
		Array<Path, 2>& texturePaths);

	// Load multiple Static Mesh from file
	//static void LoadModel(std::filesystem::path filePath);

	inline static const Path assetsDirPath = 
		std::filesystem::current_path().parent_path() / "Assets";

private:
	Assimp::Importer _importer;
	const aiScene* _scene;

	void LoadVertices(const aiMesh* aimesh, Vector<float>& vertDest);
	void LoadIndices(const aiMesh* aimesh, Vector<uint32_t>& indDest);
	Path GetTexturePath(const struct aiMaterial* material, const char* textureType);
};