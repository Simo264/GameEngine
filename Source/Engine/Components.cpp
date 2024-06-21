#include "Components.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/OpenGL.hpp"

#include "Engine/ObjectLoader.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"

/* ---------------------------------------------------------------------------
			TransformComponent
	--------------------------------------------------------------------------- */

TransformComponent::TransformComponent()
{
	UpdateTransformation();
}
void TransformComponent::UpdateTransformation()
{
	static const mat4f I = mat4f(1.0f);
	mat4f translationMatrix = Math::Translate(I, position);
	mat4f rotationMatrix = mat4f(Quat(vec3f(Math::Radians(rotation.x), Math::Radians(rotation.y), glm::radians(rotation.z))));
	mat4f scalingMatrix = Math::Scale(I, scale);
	_transformation = translationMatrix * rotationMatrix * scalingMatrix;
}
mat4f& TransformComponent::GetTransformation()
{
	return _transformation;
}

/* ---------------------------------------------------------------------------
			StaticMeshComponent
	--------------------------------------------------------------------------- */

StaticMeshComponent::StaticMeshComponent(const fspath& filePath)
{
	modelPath = filePath;

	constexpr int components =
		3 + // position -> x,y,z
		2 +	// tc				-> u,v
		3 +	// normal		-> x,y,z
		3;	// tangent  -> x,y,z

	ObjectLoader loader(filePath);
	loader.LoadMesh(0);
	const auto mesh = loader.mesh;
	const uint32_t vertices = loader.GetNumMeshVertices();
	const uint32_t indices = loader.GetNumMeshIndices();
	const uint64_t vDataBytes = vertices * components * sizeof(float);
	const uint64_t iDataBytes = indices * sizeof(uint32_t);

	Buffer vbo(GL_ARRAY_BUFFER, vDataBytes, nullptr, GL_STATIC_DRAW);
	Buffer ebo(GL_ELEMENT_ARRAY_BUFFER, iDataBytes, nullptr, GL_STATIC_DRAW);
	loader.LoadVertices(vbo);
	loader.LoadIndices(ebo);

	vao.Create();
	vao.numVertices = vertices;
	vao.numIndices = indices;

	vao.AttachVertexBuffer(0, vbo, 0, components * sizeof(float));
	vao.AttachElementBuffer(ebo);

	/* position */
	vao.EnableAttribute(0);
	vao.SetAttribBinding(0, 0);
	vao.SetAttribFormat(0, 3, GL_FLOAT, true, 0);
	/* texture coordinates */
	vao.EnableAttribute(1);
	vao.SetAttribBinding(1, 0);
	vao.SetAttribFormat(1, 2, GL_FLOAT, true, 3 * sizeof(float));
	/* normal */
	vao.EnableAttribute(2);
	vao.SetAttribBinding(2, 0);
	vao.SetAttribFormat(2, 3, GL_FLOAT, true, 5 * sizeof(float));
	/* tangent */
	vao.EnableAttribute(3);
	vao.SetAttribBinding(3, 0);
	vao.SetAttribFormat(3, 3, GL_FLOAT, true, 8 * sizeof(float));

	//material.diffuse = loader.GetTexture(aiTextureType_DIFFUSE);
	//material.specular = loader.GetTexture(aiTextureType_SPECULAR);
	//material.normal = loader.GetTexture(aiTextureType_HEIGHT);
}
void StaticMeshComponent::Draw()
{
	if (vao.numIndices == 0)
		Renderer::DrawArrays(GL_TRIANGLES, vao);
	else
		Renderer::DrawElements(GL_TRIANGLES, vao);
}
void StaticMeshComponent::DestroyMesh()
{
	vao.Delete();
}

/* ---------------------------------------------------------------------------
			CameraComponent
	--------------------------------------------------------------------------- */

CameraComponent::CameraComponent(const vec3f& position, float fov, float aspect, float znear, float zfar)
	: position{ position },
		fov{ fov },
		aspect{ aspect },
		zNear{ znear },
		zFar{ zfar },
		yaw{ -90.0f },
		pitch{ 0.0f },
		roll{ 0.0f },
		_front{},	
		_up{},
		_right{},
		_viewMatrix{}, 
		_projectionMatrix{}
{
	/* Update vectors */
	UpdateVectors();

	/* Update matrices */
	UpdateView();
	UpdateProjection();
}
void CameraComponent::UpdateVectors()
{
	const static vec3f WorldUp = vec3f(0.0f, 1.0f, 0.0f);

	vec3f calcFront{};
	calcFront.x = Math::Cos(Math::Radians(yaw)) * cos(Math::Radians(pitch));
	calcFront.y = Math::Sin(Math::Radians(pitch));
	calcFront.z = Math::Sin(Math::Radians(yaw)) * cos(Math::Radians(pitch));

	_front = Math::Normalize(calcFront);
	_right = Math::Normalize(Math::Cross(_front, WorldUp));

	const mat4f rollMat = Math::Rotate(mat4f(1.0f), Math::Radians(roll), _front);
	_up = Math::Normalize(Math::Cross(_right, _front));
	_up = mat3f(rollMat) * _up;
}
void CameraComponent::UpdateView()
{
	_viewMatrix = Math::LookAt(position, position + _front, _up);
}
void CameraComponent::UpdateProjection()
{
	_projectionMatrix = Math::Perspective(fov, aspect, zNear, zFar);
}
