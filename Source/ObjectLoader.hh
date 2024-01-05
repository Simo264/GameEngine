#pragma once

#include "Core.hh"
#include "UncopyableObject.hh"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Load mesh data from .obj files inside Assets directory
// -------------------------------------------------
class ObjectLoader : public UncopyableObject
{
public:
	ObjectLoader(Path filePath);

	// Load Static Mesh object from file
	void LoadStaticMesh(class StaticMesh* staticMesh);

	// Load multiple Mesh objects from file to create a Model object
	void LoadModel(class Model* model);
	
private:
	inline static Path _assetsDirPath = std::filesystem::current_path().parent_path() / "Assets";

	Assimp::Importer _importer;
	const aiScene* _scene;

	void LoadMesh(const aiMesh* aimesh, class Mesh* mesh);
	void LoadVertices(const aiMesh* aimesh, Vector<float>& vertDest);
	void LoadIndices(const aiMesh* aimesh, Vector<uint32_t>& indDest);
	class Texture2D* GetTexture(const aiMaterial* material, const char* textureType);
};