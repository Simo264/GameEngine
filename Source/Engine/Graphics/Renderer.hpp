#pragma once

#include "Core/Core.hpp"

class VertexArray;

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