#pragma once

#include "../RootComponent.hh"
#include "../UncopyableObject.hh"
#include "Mesh.hh"


// A model that contains multiple meshes, possibly with multiple textures. 
// A house, that contains a wooden balcony, a tower, and perhaps a swimming pool, 
// could still be loaded as a single model.
// We'll load the model via Assimp and translate it to multiple Mesh objects
class Model : public RootComponent, public UncopyableObject
{
public:
	Model(Path filePath);

	// Allocate memory for mesh objects
	void InitModel(uint32_t nMeshes);
	
	// Free memory and destroy all mesh objects
	void DestroyModel();
	
	void Draw(class Shader* shader) const;

	//const uint32_t& NumMeshes() const { return _nMeshes; }
	//StaticMesh& GetMesh(uint32_t i) { return _meshPool[i]; }
	
	UniquePointer<Mesh[]> meshBuffer;

private:
	uint32_t _nMeshes;
};
