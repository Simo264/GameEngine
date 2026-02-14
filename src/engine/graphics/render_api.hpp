#pragma once

#include "Core/Types.hpp"
#include "Types.hpp"

struct VertexArray;

namespace Components
{
	struct StaticMesh;
}

// Low-level OpenGL wrapper. Does NOT know about entities, materials, or scenes.
namespace RenderAPI
{
	// ===================================
	//				Stencil test
	// ===================================

	/** @brief Enable stencil testing and update the stencil buffer */
	void EnableStencilTest();
	/** @brief Disable stencil testing */
	void DisableStencilTest();
	/** @brief Specifies the conditions under which a fragment passes the stencil test. */
	void SetStencilFun(CompareFunc fun, i32 ref, i32 mask);
	/** @brief Specifies what should happen to stencil values depending on the outcome of the stencil and depth tests. */
	void SetStencilOp(StencilOpMode sfail, StencilOpMode dpfail, StencilOpMode dppass);
	/** @brief Specifies a bit mask to enable and disable writing of individual bits in the stencil planes. */
	void SetStencilMask(i32 mask);
	void EnableWritingStencilBuffer();
	void DisableWritingStencilBuffer();

	// ===================================
	//				Depth test
	// ===================================

	/** @brief Do depth comparisons and update the depth buffer */
	void EnableDepthTest();
	/** @brief Disable depth comparisons */
	void DisableDepthTest();
	/** @brief Enable writing into the depth buffer. */
	void EnableWritingDepthBuffer();
	/** @brief Disable writing into the depth buffer. */
	void DisableWritingDepthBuffer();
	/** @brief Specify the value used for depth buffer comparisons. */
	void SetDepthFun(CompareFunc fun);

	// ===================================
	//				Face culling 
	// ===================================

	/** @brief Cull polygons based on their winding in window coordinates */
	void EnableFaceCulling();
	/** @brief Disable face culling */
	void DisableFaceCulling();
	/** @brief Specify whether front- or back-facing facets can be culled. */
	void SetCullFace(CullFaceMode mode);
	/** @brief Specifies which of the clockwise and counterclockwise facets are front-facing and back-facing. */
	void SetFrontFace(FrontFaceMode mode);

	// ===================================
	//				Draw commands
	// ===================================

	/** @brief Render primitives from array data. */
	void DrawArrays(RenderMode mode,  const VertexArray& vertexArray, u32 numVertices);
	/** @brief Render primitives from array data. */
	void DrawElements(RenderMode mode, const VertexArray& vertexArray, u32 numIndices);
	/** @brief Renders a mesh with its associated buffers. */
	void Draw(RenderMode mode, const Components::StaticMesh& mesh);
}