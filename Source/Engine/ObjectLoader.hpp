#pragma once

#include "Core/Core.hpp"
#include "Engine/Material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/**
 * Load mesh data from file
 */
class ObjectLoader
{
public:
	ObjectLoader(const Path& filePath);

	/**
	 * Read data from file and put them into buffers
	 * 
	 * @param vbo: create and initialize the vertex buffer object with vertices
	 * @param ebo: create and initialize the vertex buffer object with indices
	 */
	void LoadMesh(class VertexBuffer& vbo, class ElementBuffer& ebo);
	
	Material material;

private:
	Assimp::Importer _importer;
	const aiScene* _scene;

	void LoadVertices(const aiMesh* aimesh, class VertexBuffer& vbo);
	void LoadIndices(const	aiMesh* aimesh,	class ElementBuffer& ebo);
	class Texture2D* LoadTexture(const aiMaterial* aimaterial, const char* textureType);
};