#include "StaticMesh.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Graphics/Material.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"
#include "Engine/Subsystems/TextureManager.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

StaticMesh::StaticMesh(const fs::path& modelPath)
	: path{ modelPath },
		meshes{}
{
	String pathStr = path.string();
	CONSOLE_TRACE("Loading static mesh {}...", pathStr);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(pathStr.c_str(),
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_ImproveCacheLocality |
		aiProcess_FindDegenerates |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FindInvalidData |
		aiProcess_LimitBoneWeights |
		aiProcess_OptimizeMeshes
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		CONSOLE_ERROR("Assimp importer error: {}", importer.GetErrorString());
		return;
	}

	meshes.reserve(scene->mNumMeshes);
	ProcessNode(scene->mRootNode, scene);
}
void StaticMesh::Draw(i32 mode)
{
	for (const auto& mesh : meshes)
	{
		auto& material = mesh.material;
		material.diffuse->BindTextureUnit(0);
		material.specular->BindTextureUnit(1);
		material.normal->BindTextureUnit(2);
		mesh.Draw(mode);
	}	
}
u32 StaticMesh::TotalVertices() const
{
	return std::reduce(meshes.begin(), meshes.end(), 0, [](i32 acc, const Mesh& mesh) {
		return acc + mesh.vao.numVertices;
	});
}
u32 StaticMesh::TotalIndices() const
{
	return std::reduce(meshes.begin(), meshes.end(), 0, [](i32 acc, const Mesh& mesh) {
		return acc + mesh.vao.numIndices;
		});
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void StaticMesh::ProcessNode(aiNode* node, const aiScene* scene)
{
	/* Process all the node's meshes */
	for (i32 i = 0; i < node->mNumMeshes; i++)
	{
		Mesh& mesh = meshes.emplace_back();
		mesh.SetupAttributeFloat(0, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T, position)));
		mesh.SetupAttributeFloat(1, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T, normal)));
		mesh.SetupAttributeFloat(2, 0, VertexFormat(2, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T, uv))); 
		mesh.SetupAttributeFloat(3, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T, tangent)));

		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];

		/* Load vertices */
		Buffer vbo = LoadVertices(aimesh);
		mesh.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_N_UV_T));
		mesh.vao.numVertices = aimesh->mNumVertices;
		/* Load indices */
		Buffer ebo = LoadIndices(aimesh);
		mesh.vao.AttachElementBuffer(ebo);
		mesh.vao.numIndices = aimesh->mNumFaces * 3;

		if (scene->HasMaterials())
		{
			aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
			if (Texture2D* diffuse = GetMaterialTexture(material, static_cast<u32>(aiTextureType_DIFFUSE)))
				mesh.material.diffuse = diffuse;
			if (Texture2D* specular = GetMaterialTexture(material, static_cast<u32>(aiTextureType_SPECULAR)))
				mesh.material.specular = specular;
			if (Texture2D* normal = GetMaterialTexture(material, static_cast<u32>(aiTextureType_NORMALS)))
				mesh.material.normal = normal;
		}
	}

	/* Then do the same for each of its children */
	for (i32 i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}
Buffer StaticMesh::LoadVertices(aiMesh* aimesh)
{
	u64 size = aimesh->mNumVertices * sizeof(Vertex_P_N_UV_T);

	Buffer buffer;
	buffer.Create();
	buffer.CreateStorage(size, nullptr, GL_STATIC_DRAW);

	f32* ptr = static_cast<f32*>(buffer.MapStorage(GL_WRITE_ONLY));
	for (u32 i = 0; i < aimesh->mNumVertices; i++)
	{
		/* Position */
		*(ptr++) = static_cast<f32>(aimesh->mVertices[i].x);
		*(ptr++) = static_cast<f32>(aimesh->mVertices[i].y);
		*(ptr++) = static_cast<f32>(aimesh->mVertices[i].z);
		/* Normal */
		*(ptr++) = static_cast<f32>(aimesh->mNormals[i].x);
		*(ptr++) = static_cast<f32>(aimesh->mNormals[i].y);
		*(ptr++) = static_cast<f32>(aimesh->mNormals[i].z);
		/* Uv coordinates */
		*(ptr++) = static_cast<f32>(aimesh->mTextureCoords[0][i].x);
		*(ptr++) = static_cast<f32>(aimesh->mTextureCoords[0][i].y);
		/* Tangent */
		*(ptr++) = static_cast<f32>(aimesh->mTangents[i].x);
		*(ptr++) = static_cast<f32>(aimesh->mTangents[i].y);
		*(ptr++) = static_cast<f32>(aimesh->mTangents[i].z);
	}
	buffer.UnmapStorage();
	return buffer;
}
Buffer StaticMesh::LoadIndices(aiMesh* aimesh)
{
	u32 numIndices = aimesh->mNumFaces * 3;
	u64 size = numIndices * sizeof(u32);

	Buffer buffer;
	buffer.Create();
	buffer.CreateStorage(size, nullptr, GL_STATIC_DRAW);

	u32* ptr = static_cast<u32*>(buffer.MapStorage(GL_WRITE_ONLY));
	for (u32 i = 0; i < aimesh->mNumFaces; i++)
	{
		const aiFace& face = aimesh->mFaces[i];
		for (i32 i = 0; i < face.mNumIndices; i++)
			*(ptr++) = static_cast<u32>(face.mIndices[i]);
	}
	buffer.UnmapStorage();
	return buffer;
}
Texture2D* StaticMesh::GetMaterialTexture(aiMaterial* material, u32 textureType)
{
	aiString fileName;
	if (material->GetTexture(static_cast<aiTextureType>(textureType), 0, &fileName) == aiReturn_SUCCESS)
		return &TextureManager::Get().GetTextureByPath(Filesystem::GetTexturesPath() / fileName.C_Str());

	return nullptr;
}
