#include "Renderer.hh"

namespace Graphics
{
	uint32_t Renderer::numRenderCallsPerFrame = 0;

	void Renderer::DrawArrays(VertexArray& vertexArray)
	{
		glBindVertexArray(vertexArray.vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexArray.numVertices);
		numRenderCallsPerFrame++;

		glBindVertexArray(0);
	}

	void Renderer::DrawArraysInstanced(VertexArray& vertexArray, int nInstances)
	{
		glBindVertexArray(vertexArray.vao);
		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexArray.numVertices, nInstances);
		numRenderCallsPerFrame++;

		glBindVertexArray(0);
	}

	void Renderer::DrawIndexed(VertexArray& vertexArray)
	{
		glBindVertexArray(vertexArray.vao);
		glDrawElements(GL_TRIANGLES, vertexArray.numIndices, GL_UNSIGNED_INT, 0);
		numRenderCallsPerFrame++;

		glBindVertexArray(0);
	}

	void Renderer::DrawIndexedInstanced(VertexArray& vertexArray, int nInstances)
	{
		glBindVertexArray(vertexArray.vao);
		glDrawElementsInstanced(GL_TRIANGLES, vertexArray.numIndices, GL_UNSIGNED_INT, 0, nInstances);
		numRenderCallsPerFrame++;

		glBindVertexArray(0);
	}
}