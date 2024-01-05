#pragma once

#include "Mesh.hh"
#include "../RootComponent.hh"

// A Static Mesh is a piece of geometry that consists of a set of polygons that can be and rendered by the graphics card. 
// Static Meshes can be translated, rotated, and scaled but they cannot have their vertices animated in any way.
// These are 3D models created in external modeling applications (such as 3dsMax, Maya, Softimage, and so on) that are imported.
class StaticMesh : public RootComponent, public Mesh
{
public:
	StaticMesh() : RootComponent(), Mesh() {}
	~StaticMesh() = default;

	void Draw(class Shader* shader);
};
