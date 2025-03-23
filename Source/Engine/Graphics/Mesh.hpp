#pragma once

#include "Core/Core.hpp"

#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Graphics/Material.hpp"
#include "Engine/Graphics/Containers/VertexArray.hpp"

/**
 * @brief A mesh typically refers to a collection of vertices, edges, and faces that define the shape of a 3D object.
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

	/**
	 * @brief Creates the Vertex Array Object (VAO) for the mesh.
	 *
	 * This method initializes the VAO for the mesh by calling the `Create()` method
	 * on the `VertexArray` object. The VAO is used to store the state needed to supply
	 * vertex data to the OpenGL pipeline.
	 *
	 * @note This method should be called before any other operations on the mesh.
	 */
	void Create();

	/**
	 * @brief Destroys the Vertex Array Object (VAO) for the mesh.
	 *
	 * This method deletes the VAO associated with the mesh by calling the `Delete()` method
	 * on the `VertexArray` object. If the VAO is valid, it will be deleted and its ID will be invalidated.
	 *
	 * @note After calling this method, the VAO is no longer valid and should not be used.
	 */
	void Destroy();
	
	void Render(class Program program, RenderMode mode) const;

	void SetupAttributeFloat(i32 attribindex, i32 bindingindex, VertexFormat format) const;
	void SetupAttributeInteger(i32 attribindex, i32 bindingindex, VertexFormat format) const;
	void SetupAttributeLong(i32 attribindex, i32 bindingindex, VertexFormat format) const;

	/** @brief Unique material instance assigned to the mesh. */
	Material material;

	/** @brief VertexArray object that stores the mesh's vertex data. */
	VertexArray vao; 

	/** @brief Number of vertices in the mesh. */
	u32 numVertices;

	/** @brief Number of indices in the mesh. */
	u32 numIndices;
};
