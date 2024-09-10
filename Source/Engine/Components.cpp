#include "Components.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Math/Extensions.hpp"

#include "Engine/Globals.hpp"

#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <numeric>

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
		mat4f rotationMatrix = mat4f(quat(vec3f(Math::Radians(rotation.x), Math::Radians(rotation.y), glm::radians(rotation.z))));
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
	void Mesh::DestroyMesh()
	{
		vao.Delete();

		material.diffuse = nullptr;
		material.specular = nullptr;
		material.normal = nullptr;
		material.height = nullptr;
	}
	void Mesh::DrawMesh(i32 mode)
	{
		if (vao.numIndices == 0)
			Renderer::DrawArrays(mode, vao);
		else
			Renderer::DrawElements(mode, vao);
	}

	void Mesh::Init()
	{
		/* Set up vertex attributes */
		/* ------------------------ */
		vao.Create();
		
		/* Position */
		vao.EnableAttribute(0);
		vao.SetAttribBinding(0, 0);
		vao.SetAttribFormat(0, 3, GL_FLOAT, true, offsetof(Vertex_P_N_UV_T, position));
		/* Normal */
		vao.EnableAttribute(1);
		vao.SetAttribBinding(1, 0);
		vao.SetAttribFormat(1, 3, GL_FLOAT, true, offsetof(Vertex_P_N_UV_T, normal));
		/* Uv coordinates */
		vao.EnableAttribute(2);
		vao.SetAttribBinding(2, 0);
		vao.SetAttribFormat(2, 2, GL_FLOAT, true, offsetof(Vertex_P_N_UV_T, uv));
		/* Tangent */
		vao.EnableAttribute(3);
		vao.SetAttribBinding(3, 0);
		vao.SetAttribFormat(3, 3, GL_FLOAT, true, offsetof(Vertex_P_N_UV_T, tangent));

		TextureManager& textureManager = TextureManager::Get();
		material.diffuse = const_cast<Texture2D*>(&textureManager.GetDefaultDiffuse());
		material.specular = const_cast<Texture2D*>(&textureManager.GetDefaultSpecular());
		material.normal = const_cast<Texture2D*>(&textureManager.GetDefaultNormal());
		material.height = const_cast<Texture2D*>(&textureManager.GetDefaultHeight());
	}


	/* ---------------------------------------------------------------------------
				ModelComponent
		--------------------------------------------------------------------------- */

	static u32 totalVertices = 0;
	static u32 totalIndices = 0;
	static u32 totalSize = 0;
	static chrono::steady_clock::time_point timeStart;
	static chrono::steady_clock::time_point timeEnd;

	Model::Model(const fs::path& path)
		: path{ path }
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
		meshes.reserve(scene->mNumMeshes);

		ProcessNode(scene->mRootNode, scene);

		timeEnd = chrono::high_resolution_clock::now();
		chrono::duration<f64> diff = timeEnd - timeStart;

		CONSOLE_TRACE(".numMeshes={}", scene->mNumMeshes);
		CONSOLE_TRACE(".totalVertices={} ", totalVertices);
		CONSOLE_TRACE(".totalIndices={}", totalIndices);
		CONSOLE_TRACE(".totalSize={} bytes", totalSize);
		CONSOLE_TRACE(".time={}s", diff.count());
	}
	void Model::DestroyModel()
	{
		/* Destroy all meshes */
		std::for_each(meshes.begin(), meshes.end(), [&](auto& mesh) {
			mesh.DestroyMesh();
		});

		/* Destroy Vector */
		Vector<Mesh>().swap(meshes);

		CONSOLE_TRACE("Model {} destroyed", path.string());
	}
	void Model::DrawModel(i32 mode)
	{
		std::for_each(meshes.begin(), meshes.end(), [&](auto& mesh) {
			auto& material = mesh.material;
			material.diffuse->BindTextureUnit(0);
			material.specular->BindTextureUnit(1);
			material.normal->BindTextureUnit(2);
			material.height->BindTextureUnit(3);

			mesh.DrawMesh(mode);
		});
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		/* Process all the node's meshes */
		for (i32 i = 0; i < node->mNumMeshes; i++)
		{
			Mesh& mesh = meshes.emplace_back();
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];

			/* Load vertex buffer */
			const i32 numVertices = aimesh->mNumVertices;
			const u64 vSize = numVertices * sizeof(Vertex_P_N_UV_T);
			Buffer vbo(GL_ARRAY_BUFFER, vSize, nullptr, GL_STATIC_DRAW);
			LoadVertices(aimesh, vbo);
			mesh.vao.AttachVertexBuffer(0, vbo.id, 0, sizeof(Vertex_P_N_UV_T));

			/* Load index buffer */
			const i32 numIndices = std::reduce(
				aimesh->mFaces,
				aimesh->mFaces + aimesh->mNumFaces,
				0,
				[](i32 n, aiFace& face) { return n + face.mNumIndices; });
			const u64 iSize = numIndices * sizeof(u32);
			Buffer ebo(GL_ELEMENT_ARRAY_BUFFER, iSize, nullptr, GL_STATIC_DRAW);
			LoadIndices(aimesh, ebo);
			mesh.vao.AttachElementBuffer(ebo.id);

			mesh.vao.numVertices = numVertices;
			mesh.vao.numIndices = numIndices;

			totalVertices += numVertices;
			totalIndices += numIndices;
			totalSize += vSize + iSize;

			if (aimesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
				if (Texture2D* diffuse = GetTexture(material, aiTextureType_DIFFUSE))
					mesh.material.diffuse = diffuse;

				if(Texture2D* specular = GetTexture(material, aiTextureType_SPECULAR))
					mesh.material.specular = specular;

				if(Texture2D* normal = GetTexture(material, aiTextureType_NORMALS))
					mesh.material.normal = normal;

				if(Texture2D* height = GetTexture(material, aiTextureType_HEIGHT))
					mesh.material.height = height;
			}
		}

		/* Then do the same for each of its children */
		for (i32 i = 0; i < node->mNumChildren; i++)
			ProcessNode(node->mChildren[i], scene);
	}
	void Model::LoadVertices(aiMesh* aimesh, Buffer& vbo)
	{
		f32* vboPtr = static_cast<f32*>(vbo.MapStorage(GL_WRITE_ONLY));
		if (!vboPtr)
		{
			CONSOLE_WARN("Error on mapping vertex buffer storage");
			return;
		}

		for (u32 i = 0; i < aimesh->mNumVertices; i++)
		{
			/* Position */
			*(vboPtr++) = static_cast<f32>(aimesh->mVertices[i].x);
			*(vboPtr++) = static_cast<f32>(aimesh->mVertices[i].y);
			*(vboPtr++) = static_cast<f32>(aimesh->mVertices[i].z);
			/* Normal */
			*(vboPtr++) = static_cast<f32>(aimesh->mNormals[i].x);
			*(vboPtr++) = static_cast<f32>(aimesh->mNormals[i].y);
			*(vboPtr++) = static_cast<f32>(aimesh->mNormals[i].z);
			/* Uv coordinates */
			*(vboPtr++) = static_cast<f32>(aimesh->mTextureCoords[0][i].x);
			*(vboPtr++) = static_cast<f32>(aimesh->mTextureCoords[0][i].y);
			/* Tangent */
			*(vboPtr++) = static_cast<f32>(aimesh->mTangents[i].x);
			*(vboPtr++) = static_cast<f32>(aimesh->mTangents[i].y);
			*(vboPtr++) = static_cast<f32>(aimesh->mTangents[i].z);
		}
		vbo.UnmapStorage();
	}
	void Model::LoadIndices(aiMesh* aimesh, Buffer& ebo)
	{
		u32* eboPtr = static_cast<u32*>(ebo.MapStorage(GL_WRITE_ONLY));
		if (!eboPtr)
		{
			CONSOLE_WARN("Error on mapping element buffer storage");
			return;
		}

		for (u32 i = 0; i < aimesh->mNumFaces; i++)
		{
			const aiFace& face = aimesh->mFaces[i];
			for (i32 i = 0; i < face.mNumIndices; i++)
				*(eboPtr++) = static_cast<u32 >(face.mIndices[i]);
		}
		ebo.UnmapStorage();
	}
	Texture2D* Model::GetTexture(aiMaterial* material, aiTextureType type)
	{
		aiString fileName;
		if (material->GetTexture(type, 0, &fileName) == aiReturn_SUCCESS)
			return &TextureManager::Get().GetTextureByPath(GetTexturesPath() / fileName.C_Str());

		return nullptr;
	}
};