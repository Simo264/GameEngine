#include "Mesh.hh"
#include "../Graphics/Renderer.hh"

/* -----------------------------------------------------
 *          MESHDATA METHODS
 * -----------------------------------------------------
*/

MeshData::MeshData() 
	: vertDataSize{ 0 }, indDataSize{ 0 }, vertData{ nullptr }, indData{ nullptr } 
{
}

MeshData::MeshData(uint32_t vertSz, float* vData, uint32_t indSz, uint32_t* iData)
	: vertDataSize{ vertSz }, indDataSize{ indSz }, vertData{ vData }, indData{ iData } 
{
}

/* -----------------------------------------------------
 *          MESH METHODS
 * -----------------------------------------------------
*/

void Mesh::InitMesh(MeshData& data, VAConfiguration& config)
{
	vertexArray.Create(data.vertDataSize, data.vertData, data.indDataSize, data.indData, config);
}

void Mesh::DestroyMesh()
{
	vertexArray.Destroy();
}

void Mesh::Draw(Shader* shader, const mat4f& model)
{
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