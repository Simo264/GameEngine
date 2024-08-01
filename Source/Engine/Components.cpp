#include "Components.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Math/Extensions.hpp"

#include "Engine/Globals.hpp"

#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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

		int size = numVertices * sizeof(Vertex);
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

		vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex));
		vao.AttachElementBuffer(ebo);

		/* position */
		vao.EnableAttribute(0);
		vao.SetAttribBinding(0, 0);
		vao.SetAttribFormat(0, 3, GL_FLOAT, true, offsetof(Vertex, position));
		/* texture coordinates */
		vao.EnableAttribute(1);
		vao.SetAttribBinding(1, 0);
		vao.SetAttribFormat(1, 2, GL_FLOAT, true, offsetof(Vertex, texCoord));
		/* normal */
		vao.EnableAttribute(2);
		vao.SetAttribBinding(2, 0);
		vao.SetAttribFormat(2, 3, GL_FLOAT, true, offsetof(Vertex, normal));
		/* tangent */
		vao.EnableAttribute(3);
		vao.SetAttribBinding(3, 0);
		vao.SetAttribFormat(3, 3, GL_FLOAT, true, offsetof(Vertex, tangent));
	}


	/* ---------------------------------------------------------------------------
				ModelComponent
		--------------------------------------------------------------------------- */

	static uint32_t totalVertices = 0;
	static uint32_t totalIndices = 0;
	static uint32_t totalSize = 0;
	static chrono::steady_clock::time_point timeStart;
	static chrono::steady_clock::time_point timeEnd;

	Model::Model(const fspath& path)
		: modelPath{ path },
			meshes{ nullptr },
			numMeshes{ 0 }
	{
		CONSOLE_TRACE("Loading model {}...", path.string().c_str());
		timeStart = chrono::high_resolution_clock::now();

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.string().c_str(),
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace 
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			CONSOLE_ERROR("ERROR::ASSIMP::{}", importer.GetErrorString());
			return;
		}

		totalVertices = 0;
		totalIndices = 0;
		totalSize = 0;
		meshes = new Mesh[scene->mNumMeshes];

		ProcessNode(scene->mRootNode, scene);

		timeEnd = chrono::high_resolution_clock::now();
		chrono::duration<double> diff = timeEnd - timeStart;

		CONSOLE_TRACE(".numMeshes={}", numMeshes);
		CONSOLE_TRACE(".totalVertices={} ", totalVertices);
		CONSOLE_TRACE(".totalIndices={}", totalIndices);
		CONSOLE_TRACE(".totalSize={} bytes", totalSize);
		CONSOLE_TRACE(".time={}s", diff.count());
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
			auto& mesh = meshes[i];
			auto& material = mesh.material;

			if (material.diffuse)  material.diffuse->BindTextureUnit(0);  else glBindTextureUnit(0, 0);
			if (material.specular) material.specular->BindTextureUnit(1); else glBindTextureUnit(1, 0);
			if (material.normal)   material.normal->BindTextureUnit(2);   else glBindTextureUnit(2, 0);
			if (material.height)   material.height->BindTextureUnit(3);   else glBindTextureUnit(3, 0);

			mesh.DrawMesh(g_drawMode);
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
			const int vSize = numVertices * sizeof(Vertex);
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
			return g_textureManager.GetTextureByPath(TEXTURES_PATH / fileName.C_Str());

		return nullptr;
	}

};