#pragma once

#include "Core/Core.hpp"

class VertexArray;

namespace Renderer
{
	/**
	* Render primitives from array data
	*
	* @param mode: specifies what kind of primitives to render. Symbolic constants
	*							GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY,
	*							GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY,
	*							GL_TRIANGLES_ADJACENCY and GL_PATCHES
	* 
	* @param first: specifies the starting index in the enabled arrays
	*/
	void DrawArrays(uint32_t mode, VertexArray& vertexArray, i32 first = 0);

	/**
		* Draw multiple instances of a range of elements.
		*
		* @param mode: specifies what kind of primitives to render. Symbolic constants
		*							GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY,
		*							GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY,
		*							GL_TRIANGLES_ADJACENCY and GL_PATCHES
		*
		* @param nInstances: specifies the number of instances of the specified range of indices to be rendered.
		* @param first: specifies the starting index in the enabled arrays
		*/
	void DrawArraysInstanced(uint32_t mode, VertexArray& vertexArray, i32 nInstances, i32 first = 0);


	/**
		* Render primitives from array data.
		*
		* @param mode: specifies what kind of primitives to render. Symbolic constants
		*							GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY,
		*							GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY,
		*							GL_TRIANGLES_ADJACENCY and GL_PATCHES
		* 
		* @param indices: specifies a pointer to the location where the indices are stored
		* @param offset: the offset in bytes from the start of the index buffer to the location of the first index to scan
		*/
	void DrawElements(u32 mode, VertexArray& vertexArray, i32 offset = 0);

	/**
		* Draw multiple instances of a set of elements.
		*
		* @param mode: specifies what kind of primitives to render. Symbolic constants
		*							GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY,
		*							GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY,
		*							GL_TRIANGLES_ADJACENCY and GL_PATCHES
		*
		* @param nInstances: specifies the number of instances of the specified range of indices to be rendered
		* @param offset: the offset in bytes from the start of the index buffer to the location of the first index to scan
		*/
	void DrawElementsInstanced(uint32_t mode, VertexArray& vertexArray, i32 nInstances, i32 offset = 0);
}