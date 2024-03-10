#include "Systems.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include "Core/Math/Extensions.hpp"

#include "Engine/ECS/Components.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include <GLFW/glfw3.h>


/* --------------------------------------------
			TransformComponent Systems
	-------------------------------------------- */
void UpdatePosition(TransformComponent& transform, const Vec3f& position)
{
	transform.position = position;
}
void UpdateScale(TransformComponent& transform, const Vec3f& scale)
{
	transform.scale = scale;
}
void UpdateRotation(TransformComponent& transform, const Vec3f& rotation)
{
	transform.rotation = rotation;
}

/* --------------------------------------------
			CameraComponent Systems
	-------------------------------------------- */


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
void RenderLight(DirLightComponent& light, Shader& shader)
{
	const uint64_t uniformNameSize = light.uniform.size();

	/* uniformName = "DirLight.direction" */
	light.uniform.append(".direction");
	shader.SetVec3f(light.uniform.c_str(), light.direction);

	/* uniformName = "DirLight.ambient" */
	light.uniform.erase(uniformNameSize);
	light.uniform.append(".ambient");
	shader.SetVec3f(light.uniform.c_str(), light.color * light.ambient);

	/* uniformName = "DirLight.diffuse" */
	light.uniform.erase(uniformNameSize);
	light.uniform.append(".diffuse");
	shader.SetVec3f(light.uniform.c_str(), light.color * light.diffuse);

	/* uniformName = "DirLight.specular" */
	light.uniform.erase(uniformNameSize);
	light.uniform.append(".specular");
	shader.SetVec3f(light.uniform.c_str(), light.color * light.specular);

	/* uniformName = "DirLight" */
	light.uniform.erase(uniformNameSize);
}
void RenderLight(PointLightComponent& light, Shader& shader)
{
	const uint64_t uniformNameSize = light.uniform.size();

	/* shaderUName = "PointLight.position" */
	light.uniform.append(".position");
	shader.SetVec3f(light.uniform.c_str(), light.position);

	/* shaderUName = "PointLight.ambient" */
	light.uniform.erase(uniformNameSize);
	light.uniform.append(".ambient");
	shader.SetVec3f(light.uniform.c_str(), light.color * light.ambient);

	/* shaderUName = "PointLight.diffuse" */
	light.uniform.erase(uniformNameSize);
	light.uniform.append(".diffuse");
	shader.SetVec3f(light.uniform.c_str(), light.color * light.diffuse);

	/* shaderUName = "PointLight.specular" */
	light.uniform.erase(uniformNameSize);
	light.uniform.append(".specular");
	shader.SetVec3f(light.uniform.c_str(), light.color * light.specular);

	/* shaderUName = "PointLight.linear" */
	light.uniform.erase(uniformNameSize);
	light.uniform.append(".linear");
	shader.SetFloat(light.uniform.c_str(), light.linear);

	/* shaderUName = "PointLight.quadratic" */
	light.uniform.erase(uniformNameSize);
	light.uniform.append(".quadratic");
	shader.SetFloat(light.uniform.c_str(), light.quadratic);

	/* shaderUName = "PointLight" */
	light.uniform.erase(uniformNameSize);
}
void RenderLight(SpotLightComponent& light, Shader& shader)
{
	const int uniformNameSize = light.uniform.size();

	light.uniform.append(".position");
	shader.SetVec3f(light.uniform.c_str(), light.position);

	light.uniform.erase(uniformNameSize);
	light.uniform.append(".direction");
	shader.SetVec3f(light.uniform.c_str(), light.direction);

	light.uniform.erase(uniformNameSize);
	light.uniform.append(".ambient");
	shader.SetVec3f(light.uniform.c_str(), light.color * light.ambient);

	light.uniform.erase(uniformNameSize);
	light.uniform.append(".diffuse");
	shader.SetVec3f(light.uniform.c_str(), light.color * light.diffuse);

	light.uniform.erase(uniformNameSize);
	light.uniform.append(".specular");
	shader.SetVec3f(light.uniform.c_str(), light.color * light.specular);

	light.uniform.erase(uniformNameSize);
	light.uniform.append(".linear");
	shader.SetFloat(light.uniform.c_str(), light.linear);

	light.uniform.erase(uniformNameSize);
	light.uniform.append(".quadratic");
	shader.SetFloat(light.uniform.c_str(), light.quadratic);

	light.uniform.erase(uniformNameSize);
	light.uniform.append(".cutOff");
	shader.SetFloat(light.uniform.c_str(), Math::Cos(Math::Radians(light.cutOff)));

	light.uniform.erase(uniformNameSize);
}
