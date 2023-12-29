#pragma once

#include "Actor.hh"
#include "Mesh/Mesh.hh"
#include "Graphics/Shader.hh"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model 
{
public:
	Model(std::filesystem::path filePath);
	~Model() = default;

	Model(const Model&) = delete;            // delete copy constructor
	Model& operator=(const Model&) = delete; // delete assign op
	
	void Draw(Shader* shader);
	void Destroy();

	const uint32_t& NumMeshes() const { return _numMeshes; }
	Mesh& GetMesh(uint32_t i) { return _meshPool[i]; }

private:
	unique_ptr<Mesh[]> _meshPool;
	uint32_t           _numMeshes;

	void LoadModel(std::filesystem::path filePath);
	void LoadMesh(uint32_t index, const struct aiScene* scene, const struct aiMesh* aimesh);
	Texture2D* LoadTexture(const struct aiMaterial* material, const char* textureType);
};
