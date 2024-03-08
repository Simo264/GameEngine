#include "Systems.hpp"

#include "Engine/ECS/Components.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

/* --------------------------------------------
			TransformComponent Systems
	-------------------------------------------- */
void UpdatePosition(TransformComponent& component, const Vec3f& position)
{
	component.position = position;
}
void UpdateScale(TransformComponent& component, const Vec3f& scale)
{
	component.scale = scale;
}
void UpdateRotation(TransformComponent& component, const Vec3f& rotation)
{
	component.rotation = rotation;
}

/* --------------------------------------------
			Render Systems
	-------------------------------------------- */
void RenderMesh(StaticMeshComponent& mesh, const Mat4f& transform, Shader& shader)
{
	if (mesh.material.diffuse)
	{
		glActiveTexture(GL_TEXTURE0);
		mesh.material.diffuse->Bind();
	}
	if (mesh.material.specular)
	{
		glActiveTexture(GL_TEXTURE1);
		mesh.material.specular->Bind();
	}

	shader.SetMat4f(SHADER_UNIFORM_MODEL, transform);

	/* If vertex array does not contain indices call DrawArrays */
	if (mesh.vertexArray->numIndices == 0)
		Renderer::DrawArrays(*mesh.vertexArray);

	/* If vertex array does contain indices call DrawIndexed */
	else
		Renderer::DrawIndexed(*mesh.vertexArray);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0); /* unbind specular */

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0); /* unbind diffuse */
}
void RenderLight(DirLightComponent& dirLight, Shader& shader)
{
	const uint64_t uniformNameSize = dirLight.uniform.size();

	/* uniformName = "DirLight.direction" */
	dirLight.uniform.append(".direction");
	shader.SetVec3f(dirLight.uniform.c_str(), dirLight.direction);

	/* uniformName = "DirLight.ambient" */
	dirLight.uniform.erase(uniformNameSize);
	dirLight.uniform.append(".ambient");
	shader.SetVec3f(dirLight.uniform.c_str(), dirLight.color * dirLight.ambient);

	/* uniformName = "DirLight.diffuse" */
	dirLight.uniform.erase(uniformNameSize);
	dirLight.uniform.append(".diffuse");
	shader.SetVec3f(dirLight.uniform.c_str(), dirLight.color * dirLight.diffuse);

	/* uniformName = "DirLight.specular" */
	dirLight.uniform.erase(uniformNameSize);
	dirLight.uniform.append(".specular");
	shader.SetVec3f(dirLight.uniform.c_str(), dirLight.color * dirLight.specular);

	/* uniformName = "DirLight" */
	dirLight.uniform.erase(uniformNameSize);
}
void RenderLight(PointLightComponent& pointLight, Shader& shader)
{
	const uint64_t uniformNameSize = pointLight.uniform.size();

	/* shaderUName = "PointLight.position" */
	pointLight.uniform.append(".position");
	shader.SetVec3f(pointLight.uniform.c_str(), pointLight.position);

	/* shaderUName = "PointLight.ambient" */
	pointLight.uniform.erase(uniformNameSize);
	pointLight.uniform.append(".ambient");
	shader.SetVec3f(pointLight.uniform.c_str(), pointLight.color * pointLight.ambient);

	/* shaderUName = "PointLight.diffuse" */
	pointLight.uniform.erase(uniformNameSize);
	pointLight.uniform.append(".diffuse");
	shader.SetVec3f(pointLight.uniform.c_str(), pointLight.color * pointLight.diffuse);

	/* shaderUName = "PointLight.specular" */
	pointLight.uniform.erase(uniformNameSize);
	pointLight.uniform.append(".specular");
	shader.SetVec3f(pointLight.uniform.c_str(), pointLight.color * pointLight.specular);

	/* shaderUName = "PointLight.linear" */
	pointLight.uniform.erase(uniformNameSize);
	pointLight.uniform.append(".linear");
	shader.SetFloat(pointLight.uniform.c_str(), pointLight.linear);

	/* shaderUName = "PointLight.quadratic" */
	pointLight.uniform.erase(uniformNameSize);
	pointLight.uniform.append(".quadratic");
	shader.SetFloat(pointLight.uniform.c_str(), pointLight.quadratic);

	/* shaderUName = "PointLight" */
	pointLight.uniform.erase(uniformNameSize);
}
void RenderLight(SpotLightComponent& spotLight, Shader& shader)
{
	const int uniformNameSize = spotLight.uniform.size();

	spotLight.uniform.append(".position");
	shader.SetVec3f(spotLight.uniform.c_str(), spotLight.position);

	spotLight.uniform.erase(uniformNameSize);
	spotLight.uniform.append(".direction");
	shader.SetVec3f(spotLight.uniform.c_str(), spotLight.direction);

	spotLight.uniform.erase(uniformNameSize);
	spotLight.uniform.append(".ambient");
	shader.SetVec3f(spotLight.uniform.c_str(), spotLight.color * spotLight.ambient);

	spotLight.uniform.erase(uniformNameSize);
	spotLight.uniform.append(".diffuse");
	shader.SetVec3f(spotLight.uniform.c_str(), spotLight.color * spotLight.diffuse);

	spotLight.uniform.erase(uniformNameSize);
	spotLight.uniform.append(".specular");
	shader.SetVec3f(spotLight.uniform.c_str(), spotLight.color * spotLight.specular);

	spotLight.uniform.erase(uniformNameSize);
	spotLight.uniform.append(".linear");
	shader.SetFloat(spotLight.uniform.c_str(), spotLight.linear);

	spotLight.uniform.erase(uniformNameSize);
	spotLight.uniform.append(".quadratic");
	shader.SetFloat(spotLight.uniform.c_str(), spotLight.quadratic);

	spotLight.uniform.erase(uniformNameSize);
	spotLight.uniform.append(".cutOff");
	shader.SetFloat(spotLight.uniform.c_str(), Math::Cos(Math::Radians(spotLight.cutOff)));

	spotLight.uniform.erase(uniformNameSize);
}
