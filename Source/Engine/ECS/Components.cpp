#include "Components.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Platform/OpenGL/OpenGL.hpp"

#include "Engine/ObjectLoader.hpp"

#include "Engine/ECS/Systems.hpp"

#include "Engine/Graphics/Texture2D.hpp"


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

void TypeComponent::ToString(String& out) const
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

void LabelComponent::ToString(String& out) const 
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
{}

const char* TransformComponent::GetComponentName(bool lower)
{
	return (lower ? "transformcomponent" : "TransformComponent");
}

Mat4f TransformComponent::GetTransformation() const
{
	static const Mat4f I		= Mat4f(1.0f);
	Mat4f translationMatrix = Math::Translate(I, position);
	Mat4f rotationMatrix		= Mat4f(Quat(Vec3f(Math::Radians(rotation.x), Math::Radians(rotation.y), glm::radians(rotation.z))));
	Mat4f scalingMatrix			= Math::Scale(I, scale);
	return translationMatrix * rotationMatrix * scalingMatrix;
}

void TransformComponent::ToString(String& out) const
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

StaticMeshComponent::StaticMeshComponent()
{
	vertexArray = std::make_shared<VertexArray>();
}

StaticMeshComponent::StaticMeshComponent(const VertexBufferLayout& layout, const VertexBufferData& data)
{
	vertexArray = std::make_shared<VertexArray>(layout, data, GL_STATIC_DRAW);
}

StaticMeshComponent::StaticMeshComponent(const Path& objFilePath)
{
	vertexArray = std::make_shared<VertexArray>();
	modelPath = objFilePath;

	ObjectLoader loader(objFilePath);
	loader.LoadMesh(this);

	material = loader.material;
}

void StaticMeshComponent::InitMesh(const VertexBufferLayout& layout, const VertexBufferData& data) const
{
	vertexArray->InitializeBuffers(layout, data, GL_STATIC_DRAW);
}

const char* StaticMeshComponent::GetComponentName(bool lower)
{
	return (lower ? "staticmeshcomponent" : "StaticMeshComponent");
}

void StaticMeshComponent::DestroyMesh() const
{
	vertexArray->Destroy();
}

void StaticMeshComponent::ToString(String& out) const
{
	char buff[128]{};
	if (!modelPath.empty())
	{
		sprintf_s(buff, "model-path=%s\n", modelPath.string().c_str());
		out.append(buff);
	}
	if (material.diffuse)
	{
		sprintf_s(buff, "material-diffuse=%s\n", material.diffuse->texturePath.string().c_str());
		out.append(buff);
	}
	if (material.specular)
	{
		sprintf_s(buff, "material-specular=%s\n", material.specular->texturePath.string().c_str());
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

void LightComponent::ToString(String& out) const 
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

void DirLightComponent::ToString(String& out) const 
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

void PointLightComponent::ToString(String& out) const 
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

void SpotLightComponent::ToString(String& out) const
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
		aspect{ aspect }
{
	/* Default orientation */
	yaw		= -90.0f;
	pitch = 0.0f;
	roll	= 0.0f;

	/* Set default z values */
	zNear	= 0.1f;
	zFar	= 100.0f;

	/* Update orientation vectors */
	_front	= {};
	_up			= {};
	_right	= {};
	UpdateVectors();
}

Mat4f CameraComponent::GetView() const 
{ 
	return Math::LookAt(position, position + _front, _up); 
}
Mat4f CameraComponent::GetProjection() const 
{
	return Math::Perspective(fov, aspect, zNear, zFar); 
}

void CameraComponent::UpdateVectors()
{
	const static Vec3f WorldUp = Vec3f(0.0f, 1.0f, 0.0f);

	Vec3f calcFront{};
	calcFront.x = Math::Cos(Math::Radians(yaw)) * cos(Math::Radians(pitch));
	calcFront.y = Math::Sin(Math::Radians(pitch));
	calcFront.z = Math::Sin(Math::Radians(yaw)) * cos(Math::Radians(pitch));

	const Mat4f rollMat = Math::Rotate(Mat4f(1.0f), Math::Radians(roll), _front);
	_front = Math::Normalize(calcFront);
	_right = Math::Normalize(Math::Cross(_front, WorldUp));
	_up = Math::Normalize(Math::Cross(_right, _front));
	_up = Mat3f(rollMat) * _up;
}

void CameraComponent::ToString(String& out) const
{
	CONSOLE_WARN("TODO");
}
