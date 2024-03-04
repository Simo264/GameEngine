#include "StaticMeshComponent.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/ObjectLoader.hpp"
#include "Engine/ECS/TransformComponent.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

StaticMeshComponent::StaticMeshComponent()
{
	vertexArray = std::make_shared<VertexArray>();
}

StaticMeshComponent::StaticMeshComponent(const VertexBufferLayout& layout, const VertexBufferData& data)
{
	vertexArray = std::make_shared<VertexArray>(layout, data, GL_STATIC_DRAW);
}

StaticMeshComponent::StaticMeshComponent(const Path& objFilePath)
{
	vertexArray = std::make_shared<VertexArray>();

	ObjectLoader loader(objFilePath);
	loader.LoadMesh(this);

	material = loader.material;
}

void StaticMeshComponent::InitMesh(const VertexBufferLayout& layout, const VertexBufferData& data) const
{
	vertexArray->InitializeBuffers(layout, data, GL_STATIC_DRAW);
}

void StaticMeshComponent::Draw(const TransformComponent& tranform, Shader* shader) const
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

