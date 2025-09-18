#include "Mesh.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Managers/TexturesManager.hpp"

// --------------------------
//          PUBLIC           
// --------------------------

Mesh::Mesh() : 
	material{},
	vertexArray{},
	numVertices{ 0u },
	numIndices{ 0u }
{
	auto& texturesManager = TexturesManager::GetInstance();
	material.albedo = texturesManager.GetDefaultDiffuse();
	material.normalMap = texturesManager.GetDefaultNormal();
}
void Mesh::Create()
{
	vertexArray = std::make_shared<VertexArray>();
	vertexArray->Create();
}

void Mesh::Destroy()
{
	if (vertexArray.use_count() == 1 && vertexArray->IsValid())
		vertexArray->Delete();

	vertexArray.reset();
}

void Mesh::Share(Mesh& other) const
{
	other.vertexArray = vertexArray;
	other.numIndices = numIndices;
	other.numVertices = numVertices;
	other.material = material;
}

void Mesh::SetupAttributeFloat(i32 attribindex, i32 bindingindex, VertexFormat format) const
{
	vertexArray->EnableAttribute(attribindex);
	vertexArray->SetAttribBinding(attribindex, bindingindex);
	vertexArray->SetAttribFormatFLoat(attribindex, format.size, format.type, format.normalized, format.relativeoffset);
}
void Mesh::SetupAttributeInteger(i32 attribindex, i32 bindingindex, VertexFormat format) const
{
	vertexArray->EnableAttribute(attribindex);
	vertexArray->SetAttribBinding(attribindex, bindingindex);
	vertexArray->SetAttribFormatInteger(attribindex, format.size, format.type, format.relativeoffset);
}
void Mesh::SetupAttributeLong(i32 attribindex, i32 bindingindex, VertexFormat format) const
{
	vertexArray->EnableAttribute(attribindex);
	vertexArray->SetAttribBinding(attribindex, bindingindex);
	vertexArray->SetAttribFormatLong(attribindex, format.size, format.relativeoffset);
}
void Mesh::Render(Program program, RenderMode mode) const
{
	material.Bind();

	if (numIndices == 0)
		Renderer::DrawArrays(mode, *vertexArray, numVertices);
	else
		Renderer::DrawElements(mode, *vertexArray, numIndices);
}