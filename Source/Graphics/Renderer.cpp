#include "Renderer.hh"

namespace Graphics
{
	uint32_t Renderer::numRenderCallsPerFrame = 0;

	void Renderer::DrawIndexed(VertexArray& vertexArray)
	{
		glBindVertexArray(vertexArray.vao);
		glDrawElements(GL_TRIANGLES, vertexArray.numIndices, GL_UNSIGNED_INT, 0);
		numRenderCallsPerFrame++;

		glBindVertexArray(0);
	}

	void Renderer::DrawIndexedInstancing(VertexArray& vertexArray, int nInstances)
	{
		glBindVertexArray(vertexArray.vao);
		glDrawElementsInstanced(GL_TRIANGLES, vertexArray.numIndices, GL_UNSIGNED_INT, 0, nInstances);
		numRenderCallsPerFrame++;

		glBindVertexArray(0);
	}
}