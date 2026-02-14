#pragma once

#include "ComponentMacros.hpp"
#include "Engine/Graphics/Buffer.hpp"
#include "Engine/Graphics/VertexArray.hpp"
#include "Engine/VertexLayout.hpp"

namespace Components
{
	/**
	 * @brief Represents a static mesh geometry that cannot be modified after creation.
	 * The StaticMesh structure encapsulates vertex and index data for a 3D model.
	 * It is called "static" to indicate that its vertex data is immutable once
	 * created and uploaded to the GPU. This is an optimal representation for
	 * rendering objects that do not deform, such as buildings, terrain, or static
	 * props, as it allows for performance optimizations.
	 */
	DECLARE_COMPONENT(StaticMesh)
	{
		using Vertex = VertexLayout<Position, Normal, TextureCoord, Tangent>;

		StaticMesh() : 
			vertexArray{}, 
			vertexBuffer{},
			indexBuffer{},
			numVertices{ 0u }, 
			numIndices{ 0u } 
		{}
		
		// Disable copy
		StaticMesh(const StaticMesh&) = delete;
		StaticMesh& operator=(const StaticMesh&) = delete;
		// Move semantics
		StaticMesh(StaticMesh&&) = default;
		StaticMesh& operator=(StaticMesh&&) = default;

		/** @brief Creates VAO object, vertex buffer and index buffer objects */
		void Create();
		/** @brief Release GPU memory. */
		void Release();

		/** @brief Method for updating data created with DYNAMIC_STORAGE */ 
		void UpdateVertexData(u32 offset, u32 size, const void* data) const;

		/**
		 * @brief Copies vertex and index data from this StaticMesh to another StaticMesh instance.
		 * This method performs a deep copy of the mesh data by creating new immutable storage
		 * buffers in the destination mesh and copying the vertex and index buffer contents.
		 * It also copies the vertex and index counters.
		 *
		 * @param other The destination StaticMesh object that will receive the copied data
		 *
		 * @pre The destination StaticMesh must be in a valid state (Create() must have been called)
		 * @pre The destination StaticMesh must have its vertex layout already configured
		 *
		 * @note This method does NOT call Create() on the destination mesh
		 * @note This method does NOT configure the vertex attribute layout
		 *
		 * @see Create()
		 * @see VertexLayout::SetupVertexArray()
		 */
		void Copy(StaticMesh& other) const;

		VertexArray vertexArray;
		Buffer vertexBuffer;
		Buffer indexBuffer;
		u32 numVertices;
		u32 numIndices;
	};
}
