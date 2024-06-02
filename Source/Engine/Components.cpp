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

	vao.Create();

	/* position(x,y,z) normal(x,y,z) tc(x,y) */

	VertexSpecifications specs{};
	specs.bindingindex = 0;
	specs.attrindex = 0;
	specs.components = 3;
	specs.type = GL_FLOAT;
	specs.normalized = false;
	specs.relativeoffset = 0;
	vao.SetVertexSpecifications(specs);
		
	specs.bindingindex = 0;
	specs.attrindex = 1;
	specs.components = 3;
	specs.type = GL_FLOAT;
	specs.normalized = false;
	specs.relativeoffset = 3 * sizeof(float);
	vao.SetVertexSpecifications(specs);
		
	specs.bindingindex = 0;
	specs.attrindex = 2;
	specs.components = 2;
	specs.type = GL_FLOAT;
	specs.normalized = false;
	specs.relativeoffset = 6 * sizeof(float);
	vao.SetVertexSpecifications(specs);

	ObjectLoader loader(filePath);

	VertexBuffer vbo;
	ElementBuffer ebo;
	loader.LoadMesh(vbo, ebo);

	material = loader.material;
	vao.numVertices = vbo.GetSize() / (8 * sizeof(float));
	vao.numIndices	= ebo.GetSize() / sizeof(uint32_t);

	if (vbo.IsValid())
	{
		vao.AttachVertexBuffer(0, vbo, 0, 8 * sizeof(float));
		
		if (ebo.IsValid())
			vao.AttachElementBuffer(ebo);
	}
	else
	{
		CONSOLE_WARN("Invalid mesh's vertex buffer!");
	}
}
void StaticMeshComponent::DestroyMesh()
{
	vao.Delete();
}
void StaticMeshComponent::Draw()
{
	glBindTextureUnit(0, 0); /* reset diffuse */
	glBindTextureUnit(1, 0); /* reset specular */

	if (material.diffuse)
		material.diffuse->BindTextureUnit(0);

	if (material.specular)
		material.specular->BindTextureUnit(1);

	/* If vertex array does not contain indices call DrawArrays */
	if (vao.numIndices == 0)
		DrawArrays(GL_TRIANGLES, vao);

	/* If vertex array does contain indices call DrawIndexed */
	else
		DrawElements(GL_TRIANGLES, vao);
}

/* ---------------------------------------------------------------------------
			DirLightComponent
	--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
			PointLightComponent
	--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
			SpotLightComponent
	--------------------------------------------------------------------------- */

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
