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

//void TransformComponent::Format(string& out) const
//{
//	char buff[64]{};
//
//	sprintf_s(buff, "position=%.3f,%.3f,%.3f\n", position.x, position.y, position.z);
//	out.append(buff);
//
//	sprintf_s(buff, "scale=%.3f,%.3f,%.3f\n", scale.x, scale.y, scale.z);
//	out.append(buff);
//
//	sprintf_s(buff, "rotation=%.3f,%.3f,%.3f\n", rotation.x, rotation.y, rotation.z);
//	out.append(buff);
//}

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

//void StaticMeshComponent::Format(string& out) const
//{
//	char buff[128]{};
//	if (!modelPath.empty())
//	{
//		sprintf_s(buff, "model-path=%s\n", modelPath.string().c_str());
//		out.append(buff);
//	}
//	if (material.diffuse)
//	{
//		sprintf_s(buff, "material-diffuse=%s\n", material.diffuse->path.string().c_str());
//		out.append(buff);
//	}
//	if (material.specular)
//	{
//		sprintf_s(buff, "material-specular=%s\n", material.specular->path.string().c_str());
//		out.append(buff);
//	}
//}



//void LightComponent::Format(string& out) const 
//{
//	char buff[64]{};
//	sprintf_s(buff, "color=%.3f,%.3f,%.3f\n", color.x, color.y, color.z);
//	out.append(buff);
//	sprintf_s(buff, "ambient=%.3f\n", ambient);
//	out.append(buff);
//	sprintf_s(buff, "diffuse=%.3f\n", diffuse);
//	out.append(buff);
//	sprintf_s(buff, "specular=%.3f\n", specular);
//	out.append(buff);
//}

/* ---------------------------------------------------------------------------
			DirLightComponent
	--------------------------------------------------------------------------- */

void DirLightComponent::RenderLight(Program& program) const
{
	program.SetUniform3f("u_directionalLight.color", color);
	program.SetUniform1f("u_directionalLight.ambient", ambient);
	program.SetUniform1f("u_directionalLight.diffuse", diffuse);
	program.SetUniform1f("u_directionalLight.specular", specular);
	program.SetUniform3f("u_directionalLight.direction", direction);
}

//void DirLightComponent::Format(string& out) const 
//{
//	LightComponent::Format(out);
//
//	char buff[64]{};
//	sprintf_s(buff, "direction=%.3f,%.3f,%.3f\n", direction.x, direction.y, direction.z);
//	out.append(buff);
//}

/* ---------------------------------------------------------------------------
			PointLightComponent
	--------------------------------------------------------------------------- */

void PointLightComponent::RenderLight(Program& program) const
{
	//program.SetUniform3f("u_pointLight.color", color);
	//program.SetUniform1f("u_pointLight.ambient", ambient);
	//program.SetUniform1f("u_pointLight.diffuse", diffuse);
	//program.SetUniform1f("u_pointLight.specular", specular);
	//program.SetUniform3f("u_pointLight.position", position);
	//program.SetUniform1f("u_pointLight.linear", linear);
	//program.SetUniform1f("u_pointLight.quadratic", quadratic);
}

//void PointLightComponent::ToString(string& out) const 
//{
//	LightComponent::ToString(out);
//	out.append("\n");
//
//	char buff[64]{};
//	sprintf_s(buff, "position=%.3f,%.3f,%.3f\n", position.x, position.y, position.z);
//	out.append(buff);
//}

/* ---------------------------------------------------------------------------
			SpotLightComponent
	--------------------------------------------------------------------------- */

void SpotLightComponent::RenderLight(Program& program) const
{
	//program.SetUniform3f(SHADER_UNIFORM_SPOTLIGHT_COLOR, color );
	//program.SetUniform1f(SHADER_UNIFORM_SPOTLIGHT_AMBIENT, ambient);
	//program.SetUniform1f(SHADER_UNIFORM_SPOTLIGHT_DIFFUSE, diffuse);
	//program.SetUniform1f(SHADER_UNIFORM_SPOTLIGHT_SPECULAR, specular);
	//program.SetUniform3f(SHADER_UNIFORM_SPOTLIGHT_POSITION, position);
	//program.SetUniform3f(SHADER_UNIFORM_SPOTLIGHT_DIRECTION, direction);
	//program.SetUniform1f(SHADER_UNIFORM_SPOTLIGHT_LINEAR, linear);
	//program.SetUniform1f(SHADER_UNIFORM_SPOTLIGHT_QUADRATIC, quadratic);
	//program.SetUniform1f(SHADER_UNIFORM_SPOTLIGHT_CUTOFF, cutOff);
	//program.SetUniform1f(SHADER_UNIFORM_SPOTLIGHT_OUTERCUTOFF, outerCutOff);
}


//void SpotLightComponent::ToString(string& out) const
//{
//	LightComponent::ToString(out);
//	out.append("\n");
//
//	char buff[64]{};
//	sprintf_s(buff, "direction=%.3f,%.3f,%.3f\n", direction.x, direction.y, direction.z);
//	out.append(buff);
//
//	sprintf_s(buff, "cutoff=%.3f\n", cutOff);
//	out.append(buff);
//}

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
