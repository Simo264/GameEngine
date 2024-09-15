#pragma once

#include "Core/Core.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/Containers/VertexArray.hpp"

/**
 * A mesh typically refers to a collection of vertices, edges, and faces that define the shape of a 3D object.
 * A mesh is a fundamental concept in 3D computer graphics and is used to represent the geometry of objects
 * that are rendered in a scene.
 * Is composed of vertices, which are points in 3D space, connected by edges, which are line segments
 * that define the boundaries of the mesh, and faces, which are polygons formed by
 * connecting three or more vertices. These elements collectively define the shape and structure of the object
 * being rendered.
 * Meshes are often represented using vertex buffer objects (VBOs) and index buffer objects (IBOs) to efficiently
 * store and manipulate the vertex data on the GPU.
 */
class Mesh
{
public:
	Mesh();
	~Mesh() = default;

	VertexArray vao;
	Material material;

	void DestroyMesh();
	void DrawMesh(i32 mode);

private:
	void Init();
};
