#include "Renderer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Graphics/VertexArray.hpp"

extern uint32_t drawCalls;

void DrawArrays(uint32_t mode, VertexArray& vertexArray)
{
	if (vertexArray.numVertices == 0)
	{
		CONSOLE_WARN("Invalid vertices number!");
		return;
	}

	vertexArray.Bind();
	glDrawArrays(mode, 0, vertexArray.numVertices);
	drawCalls++;
	vertexArray.Unbind();
}

void DrawArraysInstanced(uint32_t mode, VertexArray& vertexArray, int nInstances)
{
	if (vertexArray.numVertices == 0)
	{
		CONSOLE_WARN("Invalid vertices number!");
		return;
	}

	vertexArray.Bind();
	glDrawArraysInstanced(mode, 0, vertexArray.numVertices, nInstances);
	drawCalls++;
	vertexArray.Unbind();
}

void DrawElements(uint32_t mode, VertexArray& vertexArray)
{
	if (vertexArray.numIndices == 0)
	{
		CONSOLE_WARN("Invalid indices number!");
		return;
	}

	vertexArray.Bind();
	glDrawElements(mode, vertexArray.numIndices, GL_UNSIGNED_INT, 0);
	drawCalls++;
	vertexArray.Unbind();
}

void DrawElementsInstanced(uint32_t mode, VertexArray& vertexArray, int nInstances)
{
	if (vertexArray.numIndices == 0)
	{
		CONSOLE_WARN("Invalid indices number!");
		return;
	}

	vertexArray.Bind();
	glDrawElementsInstanced(mode, vertexArray.numIndices, GL_UNSIGNED_INT, 0, nInstances);
	drawCalls++;
	vertexArray.Unbind();
}
