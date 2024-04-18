#include "Components.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Platform/OpenGL/OpenGL.hpp"

#include "Engine/ObjectLoader.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"

/* ---------------------------------------------------------------------------
			TypeComponent
	--------------------------------------------------------------------------- */

TypeComponent::TypeComponent(uint32_t type)
	: type{ type }
{}

const char* TypeComponent::GetComponentName(bool lower)
{
	return (lower ? "typecomponent" : "TypeComponent");
}

void TypeComponent::ToString(string& out) const
{
	char buff[32]{};
	sprintf_s(buff, "type=%d\n", type);
	out.append(buff);
}

/* ---------------------------------------------------------------------------
			LabelComponent
	--------------------------------------------------------------------------- */

LabelComponent::LabelComponent(const char* label) 
	: label{label} 
{}

const char* LabelComponent::GetComponentName(bool lower)
{
	return (lower ? "labelcomponent" : "LabelComponent");
}

void LabelComponent::ToString(string& out) const 
{
	out.append(label.c_str());
}

/* ---------------------------------------------------------------------------
			TransformComponent
	--------------------------------------------------------------------------- */

TransformComponent::TransformComponent()
	: position{ 0.0f, 0.0f, 0.0f },
		scale{ 1.0f, 1.0f, 1.0f },
		rotation{ 0.0f, 0.0f, 0.0f }
{
	UpdateTransformation();
}

const char* TransformComponent::GetComponentName(bool lower)
{
	return (lower ? "transformcomponent" : "TransformComponent");
}

void TransformComponent::UpdateTransformation()
{
	static const Mat4f I = Mat4f(1.0f);
	Mat4f translationMatrix = Math::Translate(I, position);
	Mat4f rotationMatrix = Mat4f(Quat(Vec3f(Math::Radians(rotation.x), Math::Radians(rotation.y), glm::radians(rotation.z))));
	Mat4f scalingMatrix = Math::Scale(I, scale);

	_transformation = translationMatrix * rotationMatrix * scalingMatrix;
}

Mat4f& TransformComponent::GetTransformation()
{
	return _transformation;
}

void TransformComponent::ToString(string& out) const
{
	char buff[64]{};

	sprintf_s(buff, "position=%.3f,%.3f,%.3f\n", position.x, position.y, position.z);
	out.append(buff);

	sprintf_s(buff, "scale=%.3f,%.3f,%.3f\n", scale.x, scale.y, scale.z);
	out.append(buff);

	sprintf_s(buff, "rotation=%.3f,%.3f,%.3f\n", rotation.x, rotation.y, rotation.z);
	out.append(buff);
}

/* ---------------------------------------------------------------------------
			StaticMeshComponent
	--------------------------------------------------------------------------- */

StaticMeshComponent::StaticMeshComponent(
	uint64_t				vertsize,
	const void*			vertdata,
	uint64_t				indsize,
	const uint32_t* inddata)
{
	vao.Create();

	/* position(xyz) normal(xyz) tc(xy) */

	VertexSpecifications specs{};
	specs.bindingindex = 0;
	specs.attrindex = 0;
	specs.components = 3;
	specs.type = GL_FLOAT;
	specs.normalized = true;
	specs.relativeoffset = 0;
	vao.SetVertexSpecifications(specs);

	specs.bindingindex = 0;
	specs.attrindex = 1;
	specs.components = 3;
	specs.type = GL_FLOAT;
	specs.normalized = true;
	specs.relativeoffset = 3 * sizeof(float);
	vao.SetVertexSpecifications(specs);

	specs.bindingindex = 0;
	specs.attrindex = 2;
	specs.components = 2;
	specs.type = GL_FLOAT;
	specs.normalized = true;
	specs.relativeoffset = 6 * sizeof(float);
	vao.SetVertexSpecifications(specs);

	vao.numVertices = vertsize / (8 * sizeof(float));
	vao.numIndices	= indsize / sizeof(uint32_t);
	
	if (vertsize > 0)
	{
		VertexBuffer vbo(vertsize, vertdata, GL_STATIC_DRAW);
		vao.AttachVertexBuffer(0, vbo, 0, 8 * sizeof(float));

		if (indsize > 0)
		{
			ElementBuffer ebo(indsize, inddata, GL_STATIC_DRAW);
			vao.AttachElementBuffer(ebo);
		}
	}
	else
	{
		CONSOLE_WARN("Vertex buffer empty!");
	}

}

StaticMeshComponent::StaticMeshComponent(const fspath& filePath)
{
	modelPath = filePath;

	vao.Create();

	/* position(xyz) normal(xyz) tc(xy) */

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

const char* StaticMeshComponent::GetComponentName(bool lower)
{
	return (lower ? "staticmeshcomponent" : "StaticMeshComponent");
}

void StaticMeshComponent::DrawMesh()
{
	if (material.diffuse)
		material.diffuse->BindTextureUnit(0);
	
	if (material.specular)
		material.specular->BindTextureUnit(1);

	/* If vertex array does not contain indices call DrawArrays */
	if (vao.numIndices == 0)
		Renderer::DrawArrays(vao);

	/* If vertex array does contain indices call DrawIndexed */
	else
		Renderer::DrawElements(vao);

	glBindTextureUnit(0, 0);	/* unbind diffuse */
	glBindTextureUnit(1, 0);	/* unbind specular */
}

void StaticMeshComponent::ToString(string& out) const
{
	char buff[128]{};
	if (!modelPath.empty())
	{
		sprintf_s(buff, "model-path=%s\n", modelPath.string().c_str());
		out.append(buff);
	}
	if (material.diffuse)
	{
		sprintf_s(buff, "material-diffuse=%s\n", material.diffuse->path.string().c_str());
		out.append(buff);
	}
	if (material.specular)
	{
		sprintf_s(buff, "material-specular=%s\n", material.specular->path.string().c_str());
		out.append(buff);
	}
}

/* ---------------------------------------------------------------------------
			LightComponent
	--------------------------------------------------------------------------- */

LightComponent::LightComponent(const char* uniform)
	: color{ 1.0f, 1.0f, 1.0f }, /* default white color */
		ambient{ 0.125f },
		diffuse{ 0.25f },
		specular{ 0.25f }
{
	this->uniform.reserve(64);	/* Pre allocate memory */
	this->uniform = uniform;
}

void LightComponent::ToString(string& out) const 
{
	char buff[64]{};
	sprintf_s(buff, "color=%.3f,%.3f,%.3f\n", color.x, color.y, color.z);
	out.append(buff);
	sprintf_s(buff, "ambient=%.3f\n", ambient);
	out.append(buff);
	sprintf_s(buff, "diffuse=%.3f\n", diffuse);
	out.append(buff);
	sprintf_s(buff, "specular=%.3f\n", specular);
	out.append(buff);
}

/* ---------------------------------------------------------------------------
			DirLightComponent
	--------------------------------------------------------------------------- */

DirLightComponent::DirLightComponent(const char* uniform)
	: LightComponent(uniform),
		direction{ 0.0f, -1.0f, 0.0f }
{}

const char* DirLightComponent::GetComponentName(bool lower)
{
	return (lower ? "dirlightcomponent" : "DirLightComponent");
}

void DirLightComponent::RenderLight(Shader& shader)
{
	const uint64_t uniformNameSize = uniform.size();

#if 0 
	/* uniformName = "DirLight.direction" */
	uniform.append(".direction");
	shader.SetVec3f(uniform.c_str(), direction);

	/* uniformName = "DirLight.ambient" */
	uniform.erase(uniformNameSize);
	uniform.append(".ambient");
	shader.SetVec3f(uniform.c_str(), color * ambient);

	/* uniformName = "DirLight.diffuse" */
	uniform.erase(uniformNameSize);
	uniform.append(".diffuse");
	shader.SetVec3f(uniform.c_str(), color * diffuse);

	/* uniformName = "DirLight.specular" */
	uniform.erase(uniformNameSize);
	uniform.append(".specular");
	shader.SetVec3f(uniform.c_str(), color * specular);
#endif

	/* uniformName = "DirLight" */
	uniform.erase(uniformNameSize);
}

void DirLightComponent::ToString(string& out) const 
{
	LightComponent::ToString(out);

	char buff[64]{};
	sprintf_s(buff, "direction=%.3f,%.3f,%.3f\n", direction.x, direction.y, direction.z);
	out.append(buff);
}

/* ---------------------------------------------------------------------------
			PointLightComponent
	--------------------------------------------------------------------------- */

PointLightComponent::PointLightComponent(const char* uniform)
	: LightComponent(uniform),
		position{ 0.0f, 0.0f, 0.0f },
		linear{ 0.14f },
		quadratic{ 0.07f }
{}

const char* PointLightComponent::GetComponentName(bool lower)
{
	return (lower ? "pointlightcomponent" : "PointLightComponent");
}

void PointLightComponent::RenderLight(Shader& shader)
{
	const uint64_t uniformNameSize = uniform.size();
#if 0
	/* shaderUName = "PointLight.position" */
	uniform.append(".position");
	shader.SetVec3f(uniform.c_str(), position);

	/* shaderUName = "PointLight.ambient" */
	uniform.erase(uniformNameSize);
	uniform.append(".ambient");
	shader.SetVec3f(uniform.c_str(), color * ambient);

	/* shaderUName = "PointLight.diffuse" */
	uniform.erase(uniformNameSize);
	uniform.append(".diffuse");
	shader.SetVec3f(uniform.c_str(), color * diffuse);

	/* shaderUName = "PointLight.specular" */
	uniform.erase(uniformNameSize);
	uniform.append(".specular");
	shader.SetVec3f(uniform.c_str(), color * specular);

	/* shaderUName = "PointLight.linear" */
	uniform.erase(uniformNameSize);
	uniform.append(".linear");
	shader.SetFloat(uniform.c_str(), linear);

	/* shaderUName = "PointLight.quadratic" */
	uniform.erase(uniformNameSize);
	uniform.append(".quadratic");
	shader.SetFloat(uniform.c_str(), quadratic);
#endif
	/* shaderUName = "PointLight" */
	uniform.erase(uniformNameSize);
}

void PointLightComponent::ToString(string& out) const 
{
	LightComponent::ToString(out);
	out.append("\n");

	char buff[64]{};
	sprintf_s(buff, "position=%.3f,%.3f,%.3f\n", position.x, position.y, position.z);
	out.append(buff);
}

/* ---------------------------------------------------------------------------
			SpotLightComponent
	--------------------------------------------------------------------------- */

SpotLightComponent::SpotLightComponent(const char* uniform)
	: PointLightComponent(uniform),
		direction{ 0.0f, -1.0f, 0.0f },
		cutOff{ 12.0f }
{}

const char* SpotLightComponent::GetComponentName(bool lower)
{
	return (lower ? "spotlightcomponent" : "SpotLightComponent");
}

void SpotLightComponent::RenderLight(Shader& shader)
{
	const int uniformNameSize = uniform.size();

#if 0
	uniform.append(".position");
	shader.SetVec3f(uniform.c_str(), position);

	uniform.erase(uniformNameSize);
	uniform.append(".direction");
	shader.SetVec3f(uniform.c_str(), direction);

	uniform.erase(uniformNameSize);
	uniform.append(".ambient");
	shader.SetVec3f(uniform.c_str(), color * ambient);

	uniform.erase(uniformNameSize);
	uniform.append(".diffuse");
	shader.SetVec3f(uniform.c_str(), color * diffuse);

	uniform.erase(uniformNameSize);
	uniform.append(".specular");
	shader.SetVec3f(uniform.c_str(), color * specular);

	uniform.erase(uniformNameSize);
	uniform.append(".linear");
	shader.SetFloat(uniform.c_str(), linear);

	uniform.erase(uniformNameSize);
	uniform.append(".quadratic");
	shader.SetFloat(uniform.c_str(), quadratic);

	uniform.erase(uniformNameSize);
	uniform.append(".cutOff");
	shader.SetFloat(uniform.c_str(), Math::Cos(Math::Radians(cutOff)));
#endif
	uniform.erase(uniformNameSize);
}

void SpotLightComponent::ToString(string& out) const
{
	LightComponent::ToString(out);
	out.append("\n");

	char buff[64]{};
	sprintf_s(buff, "direction=%.3f,%.3f,%.3f\n", direction.x, direction.y, direction.z);
	out.append(buff);

	sprintf_s(buff, "cutoff=%.3f\n", cutOff);
	out.append(buff);
}

/* ---------------------------------------------------------------------------
			CameraComponent
	--------------------------------------------------------------------------- */

CameraComponent::CameraComponent(const Vec3f& position, float fov, float aspect)
	: position{ position },
		fov{ fov },
		aspect{ aspect },
		yaw { -90.0f }, /* Default orientation */
		pitch{ 0.0f },
		roll{ 0.0f },
		zNear{ INITIAL_ZNEAR }, 
		zFar{ INITIAL_ZFAR },
		_front{},	/* Deafult vector values */
		_up{},
		_right{},
		_viewMatrix{}, /* Deafult matrix values */
		_projectionMatrix{}
{
	/* Update vectors */
	UpdateVectors();

	/* Update matrices */
	UpdateView();
	UpdateProjection();
}

Mat4f& CameraComponent::GetView() { return _viewMatrix; }
Mat4f& CameraComponent::GetProjection() { return _projectionMatrix; }

Vec3f& CameraComponent::GetFrontVector() { return _front; }
Vec3f& CameraComponent::GetRightVector() { return _right; }
Vec3f& CameraComponent::GetUpVector() { return _up; }

void CameraComponent::UpdateVectors()
{
	const static Vec3f WorldUp = Vec3f(0.0f, 1.0f, 0.0f);

	Vec3f calcFront{};
	calcFront.x = Math::Cos(Math::Radians(yaw)) * cos(Math::Radians(pitch));
	calcFront.y = Math::Sin(Math::Radians(pitch));
	calcFront.z = Math::Sin(Math::Radians(yaw)) * cos(Math::Radians(pitch));

	_front = Math::Normalize(calcFront);
	_right = Math::Normalize(Math::Cross(_front, WorldUp));

	const Mat4f rollMat = Math::Rotate(Mat4f(1.0f), Math::Radians(roll), _front);
	_up = Math::Normalize(Math::Cross(_right, _front));
	_up = Mat3f(rollMat) * _up;
}
void CameraComponent::UpdateView()
{
	_viewMatrix = Math::LookAt(position, position + _front, _up);
}
void CameraComponent::UpdateProjection()
{
	_projectionMatrix = Math::Perspective(fov, aspect, zNear, zFar);
}

void CameraComponent::ToString(string& out) const
{
	CONSOLE_WARN("TODO");
}
