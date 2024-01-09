#pragma once

#include "../RootComponent.hh"
#include "../UncopyableObject.hh"
#include "Mesh.hh"

// A Static Mesh is a piece of geometry that consists of multiple meshes, possibly with multiple textures. 
// Static Meshes can be translated, rotated, and scaled but they cannot have their vertices animated in any way.
// These are 3D models created in external modeling applications (such as 3dsMax, Maya, Softimage, and so on) that are imported.
class StaticMesh : public RootComponent, public UncopyableObject
{
public:
	StaticMesh() = default;
	
	// The path of object file containing all vertices and indices 
	StaticMesh(Path objFilePath);
	
	// Allocate memory for mesh objects
	void SetMeshes(uint32_t nMeshes);

	void Draw(class Shader* shader);

	// Free memory and destroy all mesh objects
	void DestroyStaticMesh();

	Mesh* GetMesh(uint32_t i) { return &_meshBuffer[i]; }
	const uint32_t& GetNumMeshes() const { return _nMeshes; }

private:
	UniquePointer<Mesh[]> _meshBuffer;
	uint32_t _nMeshes;
};
