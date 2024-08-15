#include "Renderer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Graphics/Containers/VertexArray.hpp"
#include "Engine/Globals.hpp"

namespace Renderer
{
	void DrawArrays(uint32_t mode, VertexArray& vertexArray, int first)
	{
		if (vertexArray.numVertices == 0)
			CONSOLE_WARN("Invalid vertices number!");

		vertexArray.Bind();
		glDrawArrays(mode, first, vertexArray.numVertices);
		g_drawCalls++;
		vertexArray.Unbind();
	}
	void DrawArraysInstanced(uint32_t mode, VertexArray& vertexArray, int nInstances, int first)
	{
		if (vertexArray.numVertices == 0)
			CONSOLE_WARN("Invalid vertices number!");

		vertexArray.Bind();
		glDrawArraysInstanced(mode, first, vertexArray.numVertices, nInstances);
		g_drawCalls++;
		vertexArray.Unbind();
	}
	void DrawElements(uint32_t mode, VertexArray& vertexArray, int offset)
	{
		if (vertexArray.numIndices == 0)
			CONSOLE_WARN("Invalid indices number!");

		vertexArray.Bind();
		glDrawElements(mode, vertexArray.numIndices, GL_UNSIGNED_INT, (void*)offset);
		g_drawCalls++;
		vertexArray.Unbind();
	}
	void DrawElementsInstanced(uint32_t mode, VertexArray& vertexArray, int nInstances, int offset)
	{
		if (vertexArray.numIndices == 0)
			CONSOLE_WARN("Invalid indices number!");

		vertexArray.Bind();
		glDrawElementsInstanced(mode, vertexArray.numIndices, GL_UNSIGNED_INT, (void*)offset, nInstances);
		g_drawCalls++;
		vertexArray.Unbind();
	}
}