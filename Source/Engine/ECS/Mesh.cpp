#include "Mesh.hpp"

#include "Core/GL.hpp"
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
	material.height = &textureManager.GetDefaultHeight();
}

void Mesh::SetupAttribute(i32 attribindex, i32 bindingindex, VertexFormat format) const
{
	vao.EnableAttribute(attribindex);
	vao.SetAttribBinding(attribindex, bindingindex);
	vao.SetAttribFormat(attribindex, format.size, format.type, format.normalized, format.relativeoffset);
}

void Mesh::Destroy()
{
	vao.Delete();
	material.diffuse = nullptr;
	material.specular = nullptr;
	material.normal = nullptr;
	material.height = nullptr;
}

void Mesh::Draw(i32 mode)
{
	if (vao.numIndices == 0)
		Renderer::DrawArrays(mode, vao);
	else
		Renderer::DrawElements(mode, vao);
}


/* -------------------------- */
/*          PRIVATE           */
/* -------------------------- */
