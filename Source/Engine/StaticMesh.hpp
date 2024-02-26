#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/VertexArray.hpp"
#include "Engine/SceneObject.hpp"
#include "Engine/Transform.hpp"
#include "Engine/Material.hpp"

/* ------------------------------------------------------------------------ 
	A Static Mesh is a piece of geometry with multiple textures.
	Static Meshes can be translated, rotated, and scaled but they 
	cannot have their vertices animated in any way.
	These are 3D models created in external modeling applications 
	(such as 3dsMax, Maya, Softimage, and so on) that are imported.
	------------------------------------------------------------------------ */
class StaticMesh : public SceneObject
{
public:
	/* Create static mesh from raw data */
	StaticMesh(VertexArrayData& data, VertexArrayConfig& config);
	
	/* Create static mesh from .obj file */
	StaticMesh(Path objFilePath);
	
	void Draw(class Shader* shader);
	void Destroy() const;
	
	VertexArray vertexArray;
	Transform transform;
	Path modelPath;

	Material material;
};
