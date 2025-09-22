#pragma once

#include "Core/Core.hpp"

struct VertexArray;
namespace Components
{
	struct StaticMesh;
}

enum class RenderMode : u32
{
	POINTS = 0x0000, // GL_POINTS
	LINE_STRIP = 0x0003, // GL_LINE_STRIP
	LINE_LOOP = 0x0002, // GL_LINE_LOOP
	LINES = 0x0001, // GL_LINES
	LINE_STRIP_ADJACENCY = 0x000B, // GL_LINE_STRIP_ADJACENCY
	LINES_ADJACENCY = 0x000A, // GL_LINES_ADJACENCY
	TRIANGLE_STRIP = 0x0005, // GL_TRIANGLE_STRIP
	TRIANGLE_FAN = 0x0006, // GL_TRIANGLE_FAN
	TRIANGLES = 0x0004, // GL_TRIANGLES
	TRIANGLE_STRIP_ADJACENCY = 0x000D, // GL_TRIANGLE_STRIP_ADJACENCY
	TRIANGLES_ADJACENCY = 0x000C, // GL_TRIANGLES_ADJACENCY
	PATCHES = 0x000E  // GL_PATCHES
};
enum class CompareFunc : u32
{
	NEVER = 0x0200, // GL_NEVER
	LESS = 0x0201, // GL_LESS
	EQUAL = 0x0202, // GL_EQUAL
	LEQUAL = 0x0203, // GL_LEQUAL
	GREATER = 0x0204, // GL_GREATER
	NEQUAL = 0x0205, // GL_NOTEQUAL
	GEQUAL = 0x0206, // GL_GEQUAL
	ALWAYS = 0x0207  // GL_ALWAYS
};
enum class CullFaceMode : u32
{
	FRONT = 0x0404, // GL_FRONT
	BACK = 0x0405, // GL_BACK
	FRONT_BACK = 0x0408, // GL_FRONT_AND_BACK
};
enum class FrontFaceMode : u32
{
	CCW = 0x0901, // GL_CCW
	CW = 0x0900,	// GL_CW
};
enum class StencilOpMode : u32
{
	KEEP = 0x1E00, // GL_KEEP
	ZERO = 0x0000, // GL_ZERO
	REPLACE = 0x1E01, // GL_REPLACE
	INCR = 0x1E02, // GL_INCR
	INCR_WRAP = 0x8507, // GL_INCR_WRAP
	DECR = 0x1E03, // GL_DECR
	DECR_WRAP = 0x8508, // GL_DECR_WRAP
	INVERT = 0x150A  // GL_INVERT
};

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