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
	// The objFilePath contains all vertices and indices needed to create a Mesh
	Model(Path objFilePath);

	// Allocate memory for mesh objects
	void SetMeshNumber(uint32_t nMeshes);
	
	// Free memory and destroy all mesh objects
	void DestroyModel();
	
	void Draw(class Shader* shader) const;

	UniquePointer<Mesh[]> meshBuffer;

private:
	uint32_t _nMeshes;
};
