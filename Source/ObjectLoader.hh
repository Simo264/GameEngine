#pragma once

#include "Core.hh"
#include "Texture2D.hh"

#include "Mesh/StaticMesh.hh"
#include "Mesh/Model.hh"

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
	
	// Load single Static Mesh from object file
	void LoadStaticMesh(StaticMesh* staticMesh);

	// Load multiple Static Mesh from object file to create a Model object
	void LoadModel(Model* model);
	
private:
	inline static Path _assetsDirPath = std::filesystem::current_path().parent_path() / "Assets";

	Assimp::Importer _importer;
	const aiScene* _scene;

	void LoadMesh(const aiMesh* aimesh, StaticMesh* staticMesh);
	void LoadVertices(const aiMesh* aimesh, Vector<float>& vertDest);
	void LoadIndices(const aiMesh* aimesh, Vector<uint32_t>& indDest);
	Texture2D* GetTexture(const struct aiMaterial* material, const char* textureType);
};