#include "MeshComponent.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

MeshComponent::MeshComponent()
{
	vertexArray = std::make_shared<VertexArray>();
}

void MeshComponent::InitVAO(const VertexBufferLayout& layout, const VertexBufferData& data) const
{
	vertexArray->InitializeBuffers(layout, data);
}

void MeshComponent::Draw(const TransformComponent& tranform, Shader* shader) const
{
	if (material.diffuse)
	{
		glActiveTexture(GL_TEXTURE0);
		material.diffuse->Bind();
	}
	if (material.specular)
	{
		glActiveTexture(GL_TEXTURE1);
		material.specular->Bind();
	}

	/* If vertex array does not contain indices call DrawArrays */
	if (vertexArray->numIndices == 0)
		Renderer::DrawArrays(vertexArray.get());
	
	/* If vertex array does contain indices call DrawIndexed */
	else
		Renderer::DrawIndexed(vertexArray.get());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0); /* unbind specular */

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0); /* unbind diffuse */
}

