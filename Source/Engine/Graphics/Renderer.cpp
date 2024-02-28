#include "Renderer.hpp"
#include "Engine/Graphics/Core/GL_Core.hpp"
#include "Engine/Graphics/VertexArray.hpp"

#include "Core/Log/Logger.hpp"

uint32_t Renderer::drawMode = GL_TRIANGLES;

void Renderer::DrawArrays(VertexArray* vertexArray)
{
	vertexArray->Bind();
	glDrawArrays(drawMode, 0, vertexArray->numVertices);
	drawCalls++;
	vertexArray->Unbind();
}

void Renderer::DrawArraysInstanced(VertexArray* vertexArray, int nInstances)
{
	vertexArray->Bind();
	glDrawArraysInstanced(drawMode, 0, vertexArray->numVertices, nInstances);
	drawCalls++;
	vertexArray->Unbind();
}

void Renderer::DrawIndexed(VertexArray* vertexArray)
{
	if (vertexArray->numIndices == 0)
		CONSOLE_WARN("Renderer::DrawIndexed called but no indices provided");
		
	vertexArray->Bind();
	glDrawElements(drawMode, vertexArray->numIndices, GL_UNSIGNED_INT, 0);
	drawCalls++;
	vertexArray->Unbind();
}

void Renderer::DrawIndexedInstanced(VertexArray* vertexArray, int nInstances)
{
	if (vertexArray->numIndices == 0)
		CONSOLE_WARN("Renderer::DrawIndexedInstanced called but no indices provided");

	vertexArray->Bind();
	glDrawElementsInstanced(drawMode, vertexArray->numIndices, GL_UNSIGNED_INT, 0, nInstances);
	drawCalls++;
	vertexArray->Unbind();
}
