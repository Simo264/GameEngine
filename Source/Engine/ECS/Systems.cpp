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
void UpdateCameraFov(struct CameraComponent& camera, float fov)
{
	camera.fov = fov;
	camera.projectionMatrix = Math::Perspective(camera.fov, camera.aspect, camera.zNear, camera.zFar);
}
void UpdateCameraAspect(CameraComponent& camera, float aspect)
{
	camera.aspect = aspect;
	camera.projectionMatrix = Math::Perspective(camera.fov, camera.aspect, camera.zNear, camera.zFar);
}
void UpdateCameraZNear(CameraComponent& camera, float zNear)
{
	camera.zNear = zNear;
	camera.projectionMatrix = Math::Perspective(camera.fov, camera.aspect, camera.zNear, camera.zFar);
}
void UpdateCameraZFar(CameraComponent& camera, float zFar)
{
	camera.zFar = zFar;
	camera.projectionMatrix = Math::Perspective(camera.fov, camera.aspect, camera.zNear, camera.zFar);
}

void UpdateCameraVectors(CameraComponent& camera)
{
	Vec3f calcFront{};
	calcFront.x = Math::Cos(Math::Radians(camera.yaw)) * cos(Math::Radians(camera.pitch));
	calcFront.y = Math::Sin(Math::Radians(camera.pitch));
	calcFront.z = Math::Sin(Math::Radians(camera.yaw)) * cos(Math::Radians(camera.pitch));

	camera.front = Math::Normalize(calcFront);
	camera.right = Math::Normalize(Math::Cross(camera.front, Vec3f(0.0f, 1.0f, 0.0f)));
	camera.up		= Math::Normalize(Math::Cross(camera.right, camera.front));
}
void MoveCamera(CameraComponent& camera, double deltaTime)
{
	auto& window = WindowManager::Instance();
	const float velocity = camera.movementSpeed * deltaTime;

	/* W-S */
	if (window.GetKey(GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.position += camera.front * velocity;
	}
	else if (window.GetKey(GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.position -= camera.front * velocity;
	}

	/* A-D */
	if (window.GetKey(GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.position -= camera.right * velocity;
	}
	else if (window.GetKey(GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.position += camera.right * velocity;
	}

	/* SPACE-LCTRL */
	if (window.GetKey(GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.position += camera.up * velocity;
	}
	else if (window.GetKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera.position -= camera.up * velocity;
	}
}
void RotateCamera(CameraComponent& camera, double deltaTime)
{
	auto& window = WindowManager::Instance();

	Vec2d mousePos = window.GetCursorPosition();
	Vec2i windowSize = window.GetWindowSize();
	static float lastX = ((float)windowSize.x / 2.0f);
	static float lastY = ((float)windowSize.y / 2.0f);

	static bool firstMouse = true;
	if (firstMouse)
	{
		lastX = mousePos.x;
		lastY = mousePos.y;
		firstMouse = false;
	}

	const float velocity = camera.mouseSensitivity * deltaTime * 10;

	const float xoffset = lastX - mousePos.x;
	if (xoffset < 0) /* Right */
		camera.yaw += velocity;
	else if (xoffset > 0) /* Left */
		camera.yaw -= velocity;

	const float yoffset = lastY - mousePos.y;
	if (yoffset > 0) /* Up */
		camera.pitch += velocity;
	else if (yoffset < 0) /* Down */
		camera.pitch -= velocity;

	lastX = mousePos.x;
	lastY = mousePos.y;

	/* make sure that when pitch is out of bounds, screen doesn't get flipped */
	static bool constrainPitch = true;
	if (constrainPitch)
	{
		if (camera.pitch > 89.0f)
			camera.pitch = 89.0f;
		if (camera.pitch < -89.0f)
			camera.pitch = -89.0f;
	}
}

void ProcessCameraInput(CameraComponent& camera, double deltaTime)
{
	auto& window = WindowManager::Instance();

	MoveCamera(camera, deltaTime);
	if (window.GetMouseKey(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		RotateCamera(camera, deltaTime);

	UpdateCameraVectors(camera);

	camera.viewMatrix = Math::LookAt(camera.position, camera.position + camera.front, camera.up);
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
