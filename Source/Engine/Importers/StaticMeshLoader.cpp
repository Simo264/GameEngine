#include "StaticMeshLoader.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Components/StaticMesh.hpp"

#include "Engine/Material.hpp"
#include "Engine/Vertex.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"
#include "Engine/Managers/TexturesManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ----------------------------------------------------
//										PUBLIC
// ----------------------------------------------------

void StaticMeshLoader::LoadDataFromFile(const fs::path& absolutePathToFile, StaticMesh& out)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(absolutePathToFile.string().c_str(),
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
																						aiProcess_OptimizeMeshes);
		
	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		CONSOLE_ERROR("Assimp importer error: {}", importer.GetErrorString());
		return;
	}

	out.meshArray = std::make_unique<Mesh[]>(scene->mNumMeshes);
	out.nrMeshes = 0;
	ProcessAINode(scene->mRootNode, scene, out);
}

// ----------------------------------------------------
//										PRIVATE
// ----------------------------------------------------

void StaticMeshLoader::ProcessAINode(aiNode* node, const aiScene* scene, StaticMesh& out)
{
	for (u32 i = 0; i < node->mNumMeshes; i++)
	{
		Mesh &mesh = out.meshArray[out.nrMeshes++];
		mesh.Create();
		mesh.SetupAttributeFloat(0, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T, position)));
		mesh.SetupAttributeFloat(1, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T, normal)));
		mesh.SetupAttributeFloat(2, 0, VertexFormat(2, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T, uv)));
		mesh.SetupAttributeFloat(3, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T, tangent)));
		
		aiMesh *aimesh = scene->mMeshes[node->mMeshes[i]];
		
		// Load vertices
		Buffer vbo = LoadVertices(aimesh);
		mesh.vertexArray->AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_N_UV_T));
		mesh.numVertices = aimesh->mNumVertices;
		// Load indices
		Buffer ebo = LoadIndices(aimesh);
		mesh.vertexArray->AttachElementBuffer(ebo);
		mesh.numIndices = aimesh->mNumFaces * 3;
		
		if (scene->HasMaterials())
			LoadMeshMaterial(scene, aimesh, mesh);
	}
		
	// Then do the same for each of its children
	for (u32 i = 0; i < node->mNumChildren; i++)
		ProcessAINode(node->mChildren[i], scene, out);
}
Buffer StaticMeshLoader::LoadVertices(aiMesh* aimesh)
{
	u64 size = aimesh->mNumVertices * sizeof(Vertex_P_N_UV_T);

	Buffer buffer;
	buffer.Create();
	buffer.CreateStorage(size, nullptr, BufferUsage::STATIC_DRAW);

	f32 *ptr = static_cast<f32 *>(buffer.MapStorage(BufferAccess::WRITE_ONLY));
	for (u32 i = 0; i < aimesh->mNumVertices; i++)
	{
		// Position
		*(ptr++) = static_cast<f32>(aimesh->mVertices[i].x);
		*(ptr++) = static_cast<f32>(aimesh->mVertices[i].y);
		*(ptr++) = static_cast<f32>(aimesh->mVertices[i].z);
		// Normal
		*(ptr++) = static_cast<f32>(aimesh->mNormals[i].x);
		*(ptr++) = static_cast<f32>(aimesh->mNormals[i].y);
		*(ptr++) = static_cast<f32>(aimesh->mNormals[i].z);
		// Uv coordinates
		*(ptr++) = static_cast<f32>(aimesh->mTextureCoords[0][i].x);
		*(ptr++) = static_cast<f32>(aimesh->mTextureCoords[0][i].y);
		// Tangent
		*(ptr++) = static_cast<f32>(aimesh->mTangents[i].x);
		*(ptr++) = static_cast<f32>(aimesh->mTangents[i].y);
		*(ptr++) = static_cast<f32>(aimesh->mTangents[i].z);
	}
	buffer.UnmapStorage();
	return buffer;
}
Buffer StaticMeshLoader::LoadIndices(aiMesh* aimesh)
{
	u32 numIndices = aimesh->mNumFaces * 3;
	u64 size = numIndices * sizeof(u32);

	Buffer buffer;
	buffer.Create();
	buffer.CreateStorage(size, nullptr, BufferUsage::STATIC_DRAW);

	u32 *ptr = static_cast<u32 *>(buffer.MapStorage(BufferAccess::WRITE_ONLY));
	for (u32 i = 0; i < aimesh->mNumFaces; i++)
	{
		const aiFace &face = aimesh->mFaces[i];
		for (u32 j = 0; j < face.mNumIndices; j++)
			*(ptr++) = static_cast<u32>(face.mIndices[j]);
	}
	buffer.UnmapStorage();
	return buffer;
}
void StaticMeshLoader::LoadMeshMaterial(const aiScene* scene, aiMesh* aimesh, Mesh& mesh)
{
	auto& manager = TexturesManager::GetInstance();

	aiString fileName;
	aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &fileName) == aiReturn_SUCCESS)
		mesh.material.diffuse = manager.GetOrCreateTexture(fileName.C_Str());
	if (material->GetTexture(aiTextureType_SPECULAR, 0, &fileName) == aiReturn_SUCCESS)
		mesh.material.specular = manager.GetOrCreateTexture(fileName.C_Str());
	if (material->GetTexture(aiTextureType_NORMALS, 0, &fileName) == aiReturn_SUCCESS)
		mesh.material.normal = manager.GetOrCreateTexture(fileName.C_Str());
}
