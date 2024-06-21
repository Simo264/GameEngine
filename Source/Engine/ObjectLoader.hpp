#pragma once

#include "Core/Core.hpp"
#include "Engine/Material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ObjectLoader
{
public:
	ObjectLoader(const fspath& filePath);
	~ObjectLoader() = default;

	void LoadMesh(int i);
	void LoadVertices(class Buffer& vbo) const;
	void LoadIndices(class Buffer& ebo) const;
	class Texture2D* GetTexture(aiTextureType materialType) const;

	uint32_t GetNumMeshes() const { return _scene->mNumMeshes; }
	uint32_t GetNumMeshVertices() const { return mesh->mNumVertices; }
	uint32_t GetNumMeshIndices() const { return mesh->mNumFaces * 3; }

	aiMesh* mesh;
	aiMaterial* material;

private:
	Assimp::Importer _importer;
	const aiScene* _scene;
};