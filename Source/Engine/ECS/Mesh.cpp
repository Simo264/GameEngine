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
	/* Set up vertex attributes */
	/* ------------------------ */
	vao.Create();

	/* Position */
	vao.EnableAttribute(0);
	vao.SetAttribBinding(0, 0);
	vao.SetAttribFormat(0, 3, GL_FLOAT, true, offsetof(Vertex_P_N_UV_T, position));
	/* Normal */
	vao.EnableAttribute(1);
	vao.SetAttribBinding(1, 0);
	vao.SetAttribFormat(1, 3, GL_FLOAT, true, offsetof(Vertex_P_N_UV_T, normal));
	/* Uv coordinates */
	vao.EnableAttribute(2);
	vao.SetAttribBinding(2, 0);
	vao.SetAttribFormat(2, 2, GL_FLOAT, true, offsetof(Vertex_P_N_UV_T, uv));
	/* Tangent */
	vao.EnableAttribute(3);
	vao.SetAttribBinding(3, 0);
	vao.SetAttribFormat(3, 3, GL_FLOAT, true, offsetof(Vertex_P_N_UV_T, tangent));

	TextureManager& textureManager = TextureManager::Get();
	material.diffuse = &textureManager.GetDefaultDiffuse();
	material.specular = &textureManager.GetDefaultSpecular();
	material.normal = &textureManager.GetDefaultNormal();
	material.height = &textureManager.GetDefaultHeight();
}

void Mesh::DestroyMesh()
{
	vao.Delete();

	material.diffuse = nullptr;
	material.specular = nullptr;
	material.normal = nullptr;
	material.height = nullptr;
}

void Mesh::DrawMesh(i32 mode)
{
	if (vao.numIndices == 0)
		Renderer::DrawArrays(mode, vao);
	else
		Renderer::DrawElements(mode, vao);
}


/* -------------------------- */
/*          PRIVATE           */
/* -------------------------- */
