#pragma once

#include "Core/Core.hpp"
#include "Engine/Material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* ----------------------------------------------------
	Class ObjectLoader: 
	load vertices, indices, materials data from .obj file
 ----------------------------------------------------- */
class ObjectLoader
{
public:
	ObjectLoader(const Path& filePath);

	/* Load data from file and initializes vertex array object */
	void LoadMesh(class MeshComponent* mesh);
	
	Material material;

private:
	Assimp::Importer _importer;
	const aiScene* _scene;

	void LoadVertices(const aiMesh* aimesh, uint32_t writeBuffer);
	void LoadIndices(const aiMesh* aimesh, uint32_t writeBuffer);
	class Texture2D* LoadTexture(const aiMaterial* aimaterial, const char* textureType);
};