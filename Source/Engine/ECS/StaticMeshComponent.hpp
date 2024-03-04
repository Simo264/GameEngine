#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/VertexArray.hpp"

/* ---------------------------------------------------------------------------
	A Static Mesh is a piece of geometry that consists of a set of polygons that 
	can be cached in video memory and rendered by the graphics card. 
	This allows them to be rendered efficiently, meaning they can be much more 
	complex than other types of geometry such as Brushes. 
	Since they are cached in video memory, Static Meshes can be translated, rotated, 
	and scaled, but they cannot have their vertices animated in any way.
	--------------------------------------------------------------------------- */
class StaticMeshComponent
{
public:
	/* Create empty vertex array object. You Must call InitMesh to initialize buffers */
	StaticMeshComponent();

	/* Initialize Mesh component from given data */
	StaticMeshComponent(const VertexBufferLayout& layout, const VertexBufferData& data);
	
	/* Initialize Mesh reading data from .obj file */
	StaticMeshComponent(const Path& objFilePath);
	
	/* Free resources from GPU */
	void DestroyMesh() const
	{ 
		vertexArray->Destroy(); 
	}

	/* Initialize buffers */
	void InitMesh(const VertexBufferLayout& layout, const VertexBufferData& data) const;

	/* Draw mesh */
	void Draw(const Mat4f& transform, class Shader* shader) const;

	/* By default it does not contain textures */
	Material material;

	SharedPointer<VertexArray> vertexArray;
};
