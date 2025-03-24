#include "Renderer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Graphics/Containers/VertexArray.hpp"

namespace Renderer
{
	void DrawArrays(RenderMode mode, VertexArray vertexArray, u32 numVertices)
	{
		if (numVertices == 0)
		{
			CONSOLE_WARN("Invalid vertices number!");
			return;
		}

		constexpr u32 first = 0;

		vertexArray.Bind();
		glDrawArrays(static_cast<u32>(mode), first, numVertices);
		g_drawCalls++;
	}
	
	//void DrawArraysInstanced(RenderMode mode, const VertexArray& vertexArray, i32 nInstances, i32 first)
	//{
	//	if (vertexArray.numVertices == 0)
	//	{
	//		CONSOLE_WARN("Invalid vertices number!");
	//		return;
	//	}

	//	vertexArray.Bind();
	//	glDrawArraysInstanced(static_cast<u32>(mode), first, vertexArray.numVertices, nInstances);
	//	g_drawCalls++;
	//}
	
	void DrawElements(RenderMode mode, VertexArray vertexArray, u32 numIndices)
	{
		if (numIndices == 0)
		{
			CONSOLE_WARN("Invalid indices number!");
			return;
		}

		constexpr u32 offset = 0;

		vertexArray.Bind();
		glDrawElements(static_cast<u32>(mode), 
									 numIndices, 
									 GL_UNSIGNED_INT, 
									 reinterpret_cast<void*>(offset));
		g_drawCalls++;
	}
	
	//void DrawElementsInstanced(RenderMode mode, const VertexArray& vertexArray, i32 nInstances, i32 offset)
	//{
	//	if (vertexArray.numIndices == 0)
	//		CONSOLE_WARN("Invalid indices number!");

	//	vertexArray.Bind();
	//	glDrawElementsInstanced(static_cast<u32>(mode), vertexArray.numIndices, GL_UNSIGNED_INT, (void*)offset, nInstances);
	//	g_drawCalls++;
	//}
}