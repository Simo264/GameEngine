#include "StaticMesh.hpp"

#include "Engine/ObjectLoader.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"

StaticMesh::StaticMesh(VertexArrayData& data, VertexArrayConfig& config)
{
	tagName = "Static mesh";
	vertexArray.InitVertexArray(data, config);
}

StaticMesh::StaticMesh(Path objFilePath)
{
	tagName = "Static mesh";
  modelPath = objFilePath;

	VertexArrayConfig conf;
	conf.PushAttributes({ 3,3,2 }); /* 3(position), 3(normals), 2(texCoords) */

	ObjectLoader loader(modelPath);
	loader.LoadData(&vertexArray, &conf);
	material = loader.GetMaterial();
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = transform.GetTransformation();
	shader->SetMat4f("UModel", model);
	
	if (material.diffuse)
	{
		glActiveTexture(GL_TEXTURE0);
		material.diffuse->BindTexture();
	}
	if (material.specular)
	{
		glActiveTexture(GL_TEXTURE1);
		material.specular->BindTexture();
	}

	if (vertexArray.numIndices == 0)
		Renderer::DrawArrays(&vertexArray);
	else
		Renderer::DrawIndexed(&vertexArray);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0); /* unbind specular */

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0); /* unbind diffuse */
}

void StaticMesh::Destroy() const
{
	vertexArray.DestroyVertexArray();
}
