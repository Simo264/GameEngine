#pragma once

#include "Core/Core.hpp"

class VertexArray;

namespace Renderer
{
	/**
	 * @brief
	 * Render primitives from array data. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
	 */
	void DrawArrays(uint32_t mode, VertexArray& vertexArray, i32 first = 0);

	/**
	 * @brief
	 * Draw multiple instances of a range of elements. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawArraysInstanced.xhtml
	 */
	void DrawArraysInstanced(uint32_t mode, VertexArray& vertexArray, i32 nInstances, i32 first = 0);

	/**
	 * @brief
	 * Render primitives from array data. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml
	 */
	void DrawElements(u32 mode, VertexArray& vertexArray, i32 offset = 0);

	/**
		* @brief
		* Draw multiple instances of a set of elements. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElementsInstanced.xhtml
		*/
	void DrawElementsInstanced(uint32_t mode, VertexArray& vertexArray, i32 nInstances, i32 offset = 0);
}