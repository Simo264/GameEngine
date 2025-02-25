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
	 * @brief Creates and initializes the VertexArray for the mesh.
	 * This method allocates a new VertexArray and initializes it, preparing the mesh for rendering. 
	 * The VertexArray is stored in a shared pointer (`vao`) to allow multiple Mesh instances
	 * to reference the same GPU buffers.
	 */
	void Create();

	/**
	 * @brief Releases GPU resources by deleting the associated VertexArray.
	 * This method calls `VertexArray::Delete()`, which frees all associated
	 * vertex buffers (VBOs) and the index buffer (EBO) from the GPU.
	 */
	void Destroy() const;
	
	void Draw(RenderMode mode) const;

	void SetupAttributeFloat(i32 attribindex, i32 bindingindex, VertexFormat format) const;
	void SetupAttributeInteger(i32 attribindex, i32 bindingindex, VertexFormat format) const;
	void SetupAttributeLong(i32 attribindex, i32 bindingindex, VertexFormat format) const;

	/**
	 * @brief This shared pointer ensures that multiple Mesh instances can reference
	 * the same VertexArray without duplicating GPU resources.
	 * The VertexArray contains the vertex buffer objects (VBOs) and index buffers
	 * required for rendering the mesh.
	 *
	 * In cloned meshes, this pointer references the original VertexArray stored
	 * in ModelsManager to optimize memory usage and avoid redundant GPU allocations.
	 */
	SharedPtr<VertexArray> vao;

	/**
	 * @brief Unique material instance assigned to the mesh.
	 *
	 * Each Mesh has its own Material instance, which contains texture maps such as:
	 * - Diffuse texture (`diffuse`): Defines the base color.
	 * - Specular texture (`specular`): Controls the shininess and reflection.
	 * - Normal texture (`normal`): Adds surface details without extra geometry.
	 *
	 * Even when multiple Mesh instances share the same VertexArray, they can have
	 * independent materials, allowing customization of appearance without affecting
	 * other instances.
	 */
	Material material;
};
