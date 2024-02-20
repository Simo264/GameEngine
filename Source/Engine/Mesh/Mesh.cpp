#include "Mesh.hpp"

#include "Engine/Texture2D.hpp"
#include "Engine/Shader.hpp"
#include "Engine/Renderer.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Mesh::InitMesh(VertexArrayData& data, VertexArrayConfig& config)
{
	vertexArray.InitVertexArray(data, config);
}

void Mesh::Draw()
{
	if (diffuse)
	{
		glActiveTexture(GL_TEXTURE0);
		diffuse->BindTexture();
	}
	if (specular)
	{
		glActiveTexture(GL_TEXTURE1);
		specular->BindTexture();
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

void Mesh::Destroy() const
{
	vertexArray.DestroyVertexArray();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/
