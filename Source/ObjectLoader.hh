#pragma once

#include "Core.hh"
#include "NonCopyable.hh"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Load mesh data from .obj files inside Assets directory
// -------------------------------------------------
class ObjectLoader : public NonCopyable
{
public:
	ObjectLoader(Path filePath);

	// Load Static Mesh (one or more meshes) object from file
	void LoadStaticMesh(class StaticMesh* staticMesh);
	
private:
	inline static Path _assetsDirPath = std::filesystem::current_path().parent_path() / "Assets";

	Assimp::Importer _importer;
	const aiScene* _scene;

	void LoadMeshData(const aiMesh* aimesh, class Mesh* mesh);
	void LoadVertices(const aiMesh* aimesh, uint32_t writeBuffer);
	void LoadIndices(const aiMesh* aimesh, uint32_t writeBuffer);
	class Texture2D* GetTexture(const aiMaterial* material, const char* textureType);
};