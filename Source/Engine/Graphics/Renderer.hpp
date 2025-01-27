#pragma once

#include "Core/Core.hpp"

class VertexArray;

enum class RenderMode : u32
{
	Points = 0x0000, // GL_POINTS
	LineStrip = 0x0003, // GL_LINE_STRIP
	LineLoop = 0x0002, // GL_LINE_LOOP
	Lines = 0x0001, // GL_LINES
	LineStripAdjacency = 0x000B, // GL_LINE_STRIP_ADJACENCY
	LinesAdjacency = 0x000A, // GL_LINES_ADJACENCY
	TriangleStrip = 0x0005, // GL_TRIANGLE_STRIP
	TriangleFan = 0x0006, // GL_TRIANGLE_FAN
	Triangles = 0x0004, // GL_TRIANGLES
	TriangleStripAdjacency = 0x000D, // GL_TRIANGLE_STRIP_ADJACENCY
	TrianglesAdjacency = 0x000C, // GL_TRIANGLES_ADJACENCY
	Patches = 0x000E  // GL_PATCHES
};

namespace Renderer
{
	/** @brief Render primitives from array data. */
	void DrawArrays(RenderMode mode, const VertexArray& vertexArray, i32 first = 0);

	/** @brief Draw multiple instances of a range of elements. */
	void DrawArraysInstanced(RenderMode mode, const VertexArray& vertexArray, i32 nInstances, i32 first = 0);

	/** @brief Render primitives from array data. */
	void DrawElements(RenderMode mode, const VertexArray& vertexArray, i32 offset = 0);

	/** @brief Draw multiple instances of a set of elements. */
	void DrawElementsInstanced(RenderMode mode, const VertexArray& vertexArray, i32 nInstances, i32 offset = 0);
}