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
	modelPath = objFilePath;

	ObjectLoader loader(objFilePath);
	loader.LoadMesh(this);

	material = loader.material;
}

void StaticMeshComponent::InitMesh(const VertexBufferLayout& layout, const VertexBufferData& data) const
{
	vertexArray->InitializeBuffers(layout, data, GL_STATIC_DRAW);
}

void StaticMeshComponent::ToString(String& out) const
{
	char buff[128]{};
	if (!modelPath.empty())
	{
		sprintf_s(buff, "model-path=%s\n", modelPath.string().c_str());
		out.append(buff);
	}
	if (material.diffuse)
	{
		sprintf_s(buff, "material-diffuse=%s\n", material.diffuse->texturePath.string().c_str());
		out.append(buff);
	}
	if (material.specular)
	{
		sprintf_s(buff, "material-specular=%s\n", material.specular->texturePath.string().c_str());
		out.append(buff);
	}
}

void StaticMeshComponent::Draw(const Mat4f& transform, Shader* shader) const
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
	
	shader->SetMat4f(SHADER_UNIFORM_MODEL, transform);

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

