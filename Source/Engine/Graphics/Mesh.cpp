#include "Mesh.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

/* -------------------------- */
/*          PUBLIC            */
/* -------------------------- */

Mesh::Mesh()
{
	vao.Create();

	TextureManager& textureManager = TextureManager::Get();
	material.diffuse = &textureManager.GetDefaultDiffuse();
	material.specular = &textureManager.GetDefaultSpecular();
	material.normal = &textureManager.GetDefaultNormal();
}
Mesh::~Mesh()
{
	vao.Delete();
	material.diffuse = nullptr;
	material.specular = nullptr;
	material.normal = nullptr;
}

void Mesh::SetupAttributeFloat(i32 attribindex, i32 bindingindex, VertexFormat format) const
{
	vao.EnableAttribute(attribindex);
	vao.SetAttribBinding(attribindex, bindingindex);
	vao.SetAttribFormatFLoat(attribindex, format.size, format.type, format.normalized, format.relativeoffset);
}

void Mesh::SetupAttributeInteger(i32 attribindex, i32 bindingindex, VertexFormat format) const
{
	vao.EnableAttribute(attribindex);
	vao.SetAttribBinding(attribindex, bindingindex);
	vao.SetAttribFormatInteger(attribindex, format.size, format.type, format.relativeoffset);
}

void Mesh::SetupAttributeLong(i32 attribindex, i32 bindingindex, VertexFormat format) const
{
	vao.EnableAttribute(attribindex);
	vao.SetAttribBinding(attribindex, bindingindex);
	vao.SetAttribFormatLong(attribindex, format.size, format.relativeoffset);
}

void Mesh::Draw(i32 mode) const
{
	if (vao.numIndices == 0)
		Renderer::DrawArrays(mode, vao);
	else
		Renderer::DrawElements(mode, vao);
}