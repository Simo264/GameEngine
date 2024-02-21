#pragma once

#include "Mesh.hpp"
#include "Engine/SceneObject.hpp"
#include "Engine/Transform.hpp"

/* ------------------------------------------------------------------------ 
	A Static Mesh is a piece of geometry with multiple textures.
	Static Meshes can be translated, rotated, and scaled but they 
	cannot have their vertices animated in any way.
	These are 3D models created in external modeling applications 
	(such as 3dsMax, Maya, Softimage, and so on) that are imported.
	------------------------------------------------------------------------ */
class StaticMesh : public Mesh, public SceneObject
{
public:
	/* The path of object file containing all vertices and indices */
	StaticMesh(Path objFilePath);
	
	void Draw(class Shader* shader);
	
	Transform transform;

	Path modelPath;
};
