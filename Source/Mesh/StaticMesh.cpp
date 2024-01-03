#include "StaticMesh.hh"
#include "../Graphics/Renderer.hh"

void StaticMesh::InitMesh(VAData& data, VAConfig& config)
{
	vertexArray.Create(data, config);
}

void StaticMesh::DestroyMesh()
{
	vertexArray.Destroy();
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = this->GetModel();

	shader->SetMat4f("Model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (diffuse)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse->textureID);
	}
	if (specular)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular->textureID);
	}

	if (vertexArray.numIndices == 0)
		Renderer::DrawArrays(vertexArray);
	else
		Renderer::DrawIndexed(vertexArray);
}