#include "Renderer.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Graphics/Containers/VertexArray.hpp"

namespace Renderer
{
	void DrawArrays(RenderMode mode, const VertexArray& vertexArray, i32 first)
	{
		if (vertexArray.numVertices == 0)
			CONSOLE_WARN("Invalid vertices number!");

		vertexArray.Bind();
		glDrawArrays(static_cast<u32>(mode), first, vertexArray.numVertices);
		g_drawCalls++;
		vertexArray.Unbind();
	}
	void DrawArraysInstanced(RenderMode mode, const VertexArray& vertexArray, i32 nInstances, i32 first)
	{
		if (vertexArray.numVertices == 0)
			CONSOLE_WARN("Invalid vertices number!");

		vertexArray.Bind();
		glDrawArraysInstanced(static_cast<u32>(mode), first, vertexArray.numVertices, nInstances);
		g_drawCalls++;
		vertexArray.Unbind();
	}
	void DrawElements(RenderMode mode, const VertexArray& vertexArray, i32 offset)
	{
		if (vertexArray.numIndices == 0)
			CONSOLE_WARN("Invalid indices number!");

		vertexArray.Bind();
		glDrawElements(static_cast<u32>(mode), vertexArray.numIndices, GL_UNSIGNED_INT, (void*)offset);
		g_drawCalls++;
		vertexArray.Unbind();
	}
	void DrawElementsInstanced(RenderMode mode, const VertexArray& vertexArray, i32 nInstances, i32 offset)
	{
		if (vertexArray.numIndices == 0)
			CONSOLE_WARN("Invalid indices number!");

		vertexArray.Bind();
		glDrawElementsInstanced(static_cast<u32>(mode), vertexArray.numIndices, GL_UNSIGNED_INT, (void*)offset, nInstances);
		g_drawCalls++;
		vertexArray.Unbind();
	}
}