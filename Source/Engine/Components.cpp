#include "Components.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Math/Extensions.hpp"

#include "Engine/Globals.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "GLFW/glfw3.h"

namespace Components
{
	/* ---------------------------------------------------------------------------
				TransformComponent
		--------------------------------------------------------------------------- */

	Transform::Transform()
	{
		UpdateTransformation();
	}
	void Transform::UpdateTransformation()
	{
		static const mat4f I = mat4f(1.0f);
		mat4f translationMatrix = Math::Translate(I, position);
		mat4f rotationMatrix = mat4f(Quat(vec3f(Math::Radians(rotation.x), Math::Radians(rotation.y), glm::radians(rotation.z))));
		mat4f scalingMatrix = Math::Scale(I, scale);
		_transformation = translationMatrix * rotationMatrix * scalingMatrix;
	}

	/* ---------------------------------------------------------------------------
				MeshComponent
		--------------------------------------------------------------------------- */

	Mesh::Mesh()
	{
		Init();
	}
	Mesh::Mesh(void* vertices, uint32_t numVertices, void* indices, uint32_t numIndices)
	{
		Init();

		int size = numVertices * sizeof(float) * VERTEX_SIZE;
		vbo.CreateStorage(size, vertices, GL_STATIC_DRAW);
		size = numIndices * sizeof(uint32_t);
		ebo.CreateStorage(size, indices, GL_STATIC_DRAW);

		vao.numVertices = numVertices;
		vao.numIndices = numIndices;
	}
	void Mesh::DestroyMesh()
	{
		vao.Delete();
		vbo.Delete();
		ebo.Delete();

		material.diffuse = nullptr;
		material.specular = nullptr;
		material.normal = nullptr;
		material.height = nullptr;
	}
	void Mesh::DrawMesh(int mode)
	{
		if (vao.numIndices == 0)
			Renderer::DrawArrays(mode, vao);
		else
			Renderer::DrawElements(mode, vao);
	}

	void Mesh::Init()
	{
		vao.Create();
		vbo.Create();
		vbo.target = GL_ARRAY_BUFFER;
		ebo.Create();
		ebo.target = GL_ELEMENT_ARRAY_BUFFER;

		vao.AttachVertexBuffer(0, vbo, 0, VERTEX_SIZE * sizeof(float));
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
	}


	/* ---------------------------------------------------------------------------
				ModelComponent
		--------------------------------------------------------------------------- */

	static uint32_t totalVertices = 0;
	static uint32_t totalIndices = 0;
	static uint32_t totalSize = 0;
	static double timeStart = 0;
	static double timeEnd = 0;

	Model::Model(const fspath& path)
		: modelPath{ path },
			meshes{ nullptr },
			numMeshes{ 0 }
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.string().c_str(),
			aiProcess_Triangulate |
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			CONSOLE_ERROR("ERROR::ASSIMP::{}", importer.GetErrorString());
			return;
		}

		CONSOLE_TRACE("==================== Loading model {} ====================", path.string().c_str());

		totalVertices = 0;
		totalIndices = 0;
		totalSize = 0;
		meshes = new Mesh[scene->mNumMeshes];
		timeStart = glfwGetTime();

		ProcessNode(scene->mRootNode, scene);

		timeEnd = glfwGetTime();

		CONSOLE_TRACE(".numMeshes={}", numMeshes);
		CONSOLE_TRACE(".totalVertices={} ", totalVertices);
		CONSOLE_TRACE(".totalIndices={}", totalIndices);
		CONSOLE_TRACE(".totalSize={} bytes", totalSize);
		CONSOLE_TRACE(".time={}s", timeEnd - timeStart);
	}
	void Model::DestroyModel()
	{
		for (int i = 0; i < numMeshes; i++)
		{
			Mesh& mesh = meshes[i];
			mesh.DestroyMesh();
		}

		delete[] meshes;
		meshes = nullptr;
		numMeshes = 0;

		CONSOLE_TRACE("Model {} destroyed", modelPath.string().c_str());
	}
	void Model::DrawModel(int mode)
	{
		for (int i = 0; i < numMeshes; i++)
		{
			Mesh& mesh = meshes[i];

			glBindTextureUnit(0, 0);
			glBindTextureUnit(1, 0);
			glBindTextureUnit(2, 0);
			glBindTextureUnit(3, 0);

			if (mesh.material.diffuse) mesh.material.diffuse->BindTextureUnit(0);
			if (mesh.material.specular) mesh.material.specular->BindTextureUnit(1);
			if (mesh.material.normal) mesh.material.normal->BindTextureUnit(2);
			if (mesh.material.height) mesh.material.height->BindTextureUnit(3);

			mesh.DrawMesh(mode);
		}
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		/* Process all the node's meshes */
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			Mesh& mesh = meshes[numMeshes++];
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];

			const uint32_t numVertices = aimesh->mNumVertices;
			const int vSize = numVertices * sizeof(float) * VERTEX_SIZE;
			mesh.vbo.CreateStorage(vSize, nullptr, GL_STATIC_DRAW);
			mesh.vao.numVertices = numVertices;
			LoadVertices(aimesh, mesh.vbo);

			const uint32_t numIndices = std::reduce(
				aimesh->mFaces,
				aimesh->mFaces + aimesh->mNumFaces,
				0,
				[](int n, aiFace& face) { return n + face.mNumIndices; });

			const uint32_t iSize = numIndices * sizeof(uint32_t);
			mesh.ebo.CreateStorage(iSize, nullptr, GL_STATIC_DRAW);
			mesh.vao.numIndices = numIndices;
			LoadIndices(aimesh, mesh.ebo);

			totalVertices += numVertices;
			totalIndices += numIndices;
			totalSize += vSize + iSize;

			if (aimesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
				mesh.material.diffuse = GetTexture(material, aiTextureType_DIFFUSE);
				mesh.material.specular = GetTexture(material, aiTextureType_SPECULAR);
				mesh.material.normal = GetTexture(material, aiTextureType_NORMALS);
				mesh.material.height = GetTexture(material, aiTextureType_HEIGHT);
			}
		}

		/* Then do the same for each of its children */
		for (int i = 0; i < node->mNumChildren; i++)
			ProcessNode(node->mChildren[i], scene);
	}
	void Model::LoadVertices(aiMesh* aimesh, Buffer& vbo)
	{
		float* vboPtr = static_cast<float*>(vbo.MapStorage(GL_WRITE_ONLY));
		if (!vboPtr)
		{
			CONSOLE_WARN("Error on mapping vertex buffer storage");
			return;
		}

		for (uint32_t i = 0; i < aimesh->mNumVertices; i++)
		{
			/* position */
			*(vboPtr++) = static_cast<float>(aimesh->mVertices[i].x);
			*(vboPtr++) = static_cast<float>(aimesh->mVertices[i].y);
			*(vboPtr++) = static_cast<float>(aimesh->mVertices[i].z);
			/* texture coordinates */
			*(vboPtr++) = static_cast<float>(aimesh->mTextureCoords[0][i].x);
			*(vboPtr++) = static_cast<float>(aimesh->mTextureCoords[0][i].y);
			/* normal */
			*(vboPtr++) = static_cast<float>(aimesh->mNormals[i].x);
			*(vboPtr++) = static_cast<float>(aimesh->mNormals[i].y);
			*(vboPtr++) = static_cast<float>(aimesh->mNormals[i].z);
			/* tangent */
			*(vboPtr++) = static_cast<float>(aimesh->mTangents[i].x);
			*(vboPtr++) = static_cast<float>(aimesh->mTangents[i].y);
			*(vboPtr++) = static_cast<float>(aimesh->mTangents[i].z);
		}
		vbo.UnmapStorage();
	}
	void Model::LoadIndices(aiMesh* aimesh, Buffer& ebo)
	{
		uint32_t* eboPtr = static_cast<uint32_t*>(ebo.MapStorage(GL_WRITE_ONLY));
		if (!eboPtr)
		{
			CONSOLE_WARN("Error on mapping element buffer storage");
			return;
		}

		for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
		{
			const aiFace& face = aimesh->mFaces[i];
			for (int i = 0; i < face.mNumIndices; i++)
				*(eboPtr++) = static_cast<uint32_t>(face.mIndices[i]);
		}
		ebo.UnmapStorage();
	}
	Texture2D* Model::GetTexture(aiMaterial* material, aiTextureType type)
	{
		aiString fileName;
		if (material->GetTexture(type, 0, &fileName) == aiReturn_SUCCESS)
			return TextureManager::Instance()->GetTextureByPath(TEXTURES_PATH / fileName.C_Str());

		return nullptr;
	}
	
	/* ---------------------------------------------------------------------------
				CameraComponent
		--------------------------------------------------------------------------- */

	Camera::Camera(const vec3f& position, float fov, float aspect, float znear, float zfar)
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
	void Camera::UpdateVectors()
	{
		static const vec3f WorldUp = vec3f(0.0f, 1.0f, 0.0f);

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
	void Camera::UpdateView()
	{
		_viewMatrix = Math::LookAt(position, position + _front, _up);
	}
	void Camera::UpdateProjection()
	{
		_projectionMatrix = Math::Perspective(fov, aspect, zNear, zFar);
	}
};