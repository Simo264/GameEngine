#pragma once

#include "Core.hh"
#include "NonCopyable.hh"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* ----------------------------------------------------
	Class ObjectLoader: 
	load vertices, indices, materials data from .obj file
 ----------------------------------------------------- */
class ObjectLoader : public NonCopyable
{
public:
	ObjectLoader(Path filePath);
	~ObjectLoader() = default;

	/* Load data from file and initializes mesh */
	void LoadMesh(class Mesh* mesh, uint32_t meshIndex = 0);
	
private:
	Assimp::Importer _importer;
	const aiScene* _scene;
	
	void LoadVertices(const aiMesh* aimesh, uint32_t writeBuffer);
	void LoadIndices(const aiMesh* aimesh, uint32_t writeBuffer);
	void LoadMaterials(const aiMesh* aimesh, class Mesh* mesh);
	class Texture2D* GetTexture(const aiMaterial* material, const char* textureType);
};