#pragma once

#include "../Core.hh"
#include "../RootComponent.hh"
#include "../Graphics/Shader.hh"
#include "StaticMesh.hh"

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
	Model(Path filePath);
	~Model() = default;

	Model(const Model&) = delete;            // delete copy constructor
	Model& operator=(const Model&) = delete; // delete assign op
	
	void Draw(Shader* shader);
	void Destroy();

	// Allocate memory for mesh objects
	void Initialize(uint32_t nMeshes);

	//const uint32_t& NumMeshes() const { return _nMeshes; }
	//StaticMesh& GetMesh(uint32_t i) { return _meshPool[i]; }
	
	UniquePointer<StaticMesh[]> meshBuffer;

private:
	uint32_t _nMeshes;
};
