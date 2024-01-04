#pragma once

#include "Core.hh"
#include "RootComponent.hh"

#include "Mesh/StaticMesh.hh"
#include "Graphics/Shader.hh"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


// A model that contains multiple meshes, possibly with multiple textures. 
// A house, that contains a wooden balcony, a tower, and perhaps a swimming pool, 
// could still be loaded as a single model.
// We'll load the model via Assimp and translate it to multiple StaticMesh objects
class Model : public RootComponent
{
public:
	Model(std::filesystem::path filePath);
	~Model() = default;

	Model(const Model&) = delete;            // delete copy constructor
	Model& operator=(const Model&) = delete; // delete assign op
	
	//void Draw(Shader* shader);
	//void Destroy();

	//const uint32_t& NumMeshes() const { return _numMeshes; }
	//StaticMesh& GetMesh(uint32_t i) { return _meshPool[i]; }

private:
	//unique_ptr<StaticMesh[]> _meshPool;
	//uint32_t _numMeshes;

	//void LoadModel(std::filesystem::path filePath);
	//void LoadMesh(uint32_t index, const struct aiScene* scene, const struct aiMesh* aimesh);
	//Texture2D* LoadTexture(const struct aiMaterial* material, const char* textureType);
};
