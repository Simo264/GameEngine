#pragma once

#include "Core/Core.hpp"

#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/Containers/VertexArray.hpp"

/**
 * @brief Represents a 3D mesh used in graphics rendering.
 *
 * A mesh is a collection of vertices, edges, and faces that define a 3D object.
 * In computer graphics, meshes are essential for representing models, and they
 * are often stored and manipulated using Vertex Array Objects (VAO).
 *
 * The Mesh class manages these elements efficiently, supporting shared vertex
 * array references to optimize memory usage and rendering performance.
 */
class Mesh
{
public:
	Mesh();
	~Mesh() = default;

	/**
	 * @brief Creates the Vertex Array Object (VAO) for the mesh.
	 * @note This method should be called before any other operations on the mesh.
	 */
	void Create();
	
	/** @brief Releases the GPU resources used by this mesh if it's the last owner. */
	void Destroy();

	/**
	 * @brief Shares the mesh data with another instance.
	 * Instead of creating independent copies, it shares vertexArray reference,
	 * allowing both instances to point to the same data.
	 *
	 * @param other The target Mesh instance that will receive the shared data.
	 */
	void Share(Mesh& other) const;

	/**
	 * @brief Renders the mesh using the specified shader program and render mode.
	 * Binds the material associated with the mesh before rendering.
	 *
	 * @param program The shader program used for rendering.
	 * @param mode The rendering mode applied to the mesh.
	 */
	void Render(class Program program, RenderMode mode) const;

	/** @brief Configures a floating-point vertex attribute for the mesh. */
	void SetupAttributeFloat(i32 attribindex, i32 bindingindex, VertexFormat format) const;
	/** @brief Configures an integer vertex attribute for the mesh. */
	void SetupAttributeInteger(i32 attribindex, i32 bindingindex, VertexFormat format) const;
	/** @brief Configures a long integer vertex attribute for the mesh. */
	void SetupAttributeLong(i32 attribindex, i32 bindingindex, VertexFormat format) const;

	/** @brief Unique material instance assigned to the mesh. */
	Material material;

	/**
	 * @brief Shared reference to the Vertex Array Object used by this mesh.
	 * It is shared between identical meshes to reduce memory usage and avoid redundant OpenGL state setup.
	 */
	SharedPointer<VertexArray> vertexArray;

	/** @brief Number of vertices in the mesh. */
	u32 numVertices;

	/** @brief Number of indices in the mesh. */
	u32 numIndices;
};
