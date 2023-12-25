#include "Renderer.hh"
#include "../Logger.hh"

uint32_t Renderer::drawCalls = 0;

void Renderer::DrawArrays(VertexArray& vertexArray)
{
	vertexArray.Bind();
	glDrawArrays(GL_TRIANGLES, 0, vertexArray.numVertices);
	drawCalls++;

	vertexArray.Unbind();
}

void Renderer::DrawArraysInstanced(VertexArray& vertexArray, int nInstances)
{
	vertexArray.Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertexArray.numVertices, nInstances);
	drawCalls++;

	vertexArray.Unbind();
}

void Renderer::DrawIndexed(VertexArray& vertexArray)
{
	if(vertexArray.numIndices <= 0)
		CONSOLE_WARN("You called Renderer::DrawIndexed but with no indices");
		
	vertexArray.Bind();
	glDrawElements(GL_TRIANGLES, vertexArray.numIndices, GL_UNSIGNED_INT, 0);
	drawCalls++;

	vertexArray.Unbind();
}

void Renderer::DrawIndexedInstanced(VertexArray& vertexArray, int nInstances)
{
	if (vertexArray.numIndices <= 0)
		CONSOLE_WARN("You called Renderer::DrawIndexedInstanced but with no indices");

	vertexArray.Bind();
	glDrawElementsInstanced(GL_TRIANGLES, vertexArray.numIndices, GL_UNSIGNED_INT, 0, nInstances);
	drawCalls++;

	vertexArray.Unbind();
}
