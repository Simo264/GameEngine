#include "Renderer.hh"
#include "VertexArray.hh"

#include "../Logger.hh"

void Renderer::DrawArrays(VertexArray* vertexArray)
{
	vertexArray->BindVertexArray();
	glDrawArrays(GL_TRIANGLES, 0, vertexArray->numVertices);
	drawCalls++;
	vertexArray->UnbindVertexArray();
}

void Renderer::DrawArraysInstanced(VertexArray* vertexArray, int nInstances)
{
	vertexArray->BindVertexArray();
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertexArray->numVertices, nInstances);
	drawCalls++;

	vertexArray->UnbindVertexArray();
}

void Renderer::DrawIndexed(VertexArray* vertexArray)
{
	if(vertexArray->numIndices <= 0)
		CONSOLE_WARN("Renderer::DrawIndexed called but no indices provided");
		
	vertexArray->BindVertexArray();
	glDrawElements(GL_TRIANGLES, vertexArray->numIndices, GL_UNSIGNED_INT, 0);
	drawCalls++;

	vertexArray->UnbindVertexArray();
}

void Renderer::DrawIndexedInstanced(VertexArray* vertexArray, int nInstances)
{
	if (vertexArray->numIndices <= 0)
		CONSOLE_WARN("Renderer::DrawIndexedInstanced called but no indices provided");

	vertexArray->BindVertexArray();
	glDrawElementsInstanced(GL_TRIANGLES, vertexArray->numIndices, GL_UNSIGNED_INT, 0, nInstances);
	drawCalls++;

	vertexArray->UnbindVertexArray();
}
