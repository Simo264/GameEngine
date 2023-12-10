#include "Renderer.hh"

namespace Graphics
{
	uint32_t Renderer::numRenderCallsPerFrame = 0;

	void Renderer::DrawIndexed(VertexArray& vertexArray)
	{
		glBindVertexArray(vertexArray.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArray.ebo);
		glDrawElements(GL_TRIANGLES, vertexArray.numIndices, GL_UNSIGNED_INT, 0);
		numRenderCallsPerFrame++;

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}