#pragma once

#include "Core/Core.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/Containers/VertexArray.hpp"

struct VertexFormat
{
  /**
   * The number of components per vertex are allocated to the specified attribute and must be 1, 2, 3, 4, or GL_BGRA
   */
  i32 size;

  /**
   * Indicates the type of the data. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribFormat.xhtml
   */
  i32 type;

  /**
   * If true then integer data is normalized to the range [-1, 1] or [0, 1] if it is signed or unsigned, respectively.
   * Otherwise if normalized is false then integer data is directly converted to floating point
   */
  bool normalized;

  /**
   * Is the offset of the first element relative to the start of the vertex buffer binding this attribute fetches from
   */
  i32 relativeoffset;
};



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

	void SetupAttribute(i32 attribindex, i32 bindingindex, VertexFormat format) const;

	VertexArray vao;
	Material material;

	void Destroy();
	void Draw(i32 mode);
};
