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
	ObjectLoader(Path filePath);

	/* Load data from file and initializes vertex array object */
	void LoadData(class VertexArray* vao, class VertexArrayConfig* conf);
	
	Material& GetMaterial() { return _material; }

private:
	Assimp::Importer _importer;
	const aiScene* _scene;
	Material _material;

	void LoadVertices(const aiMesh* aimesh, uint32_t writeBuffer);
	void LoadIndices(const aiMesh* aimesh, uint32_t writeBuffer);
	class Texture2D* LoadTexture(const aiMaterial* material, const char* textureType);
};