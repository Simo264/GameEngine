#include "StaticMeshLoader.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Vertex.hpp"
#include "Engine/Managers/TexturesManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ----------------------------------------------------
//										PUBLIC
// ----------------------------------------------------

void StaticMeshLoader::LoadDataFromFile(const fs::path& absolutePathToFile, Components::StaticMesh& out)
{
	auto importer = Assimp::Importer{};
	auto scene = importer.ReadFile(absolutePathToFile.string().c_str(),
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
	out.nrMeshes = 0u;
	__ProcessAINode(scene->mRootNode, scene, out);
}

// ----------------------------------------------------
//										PRIVATE
// ----------------------------------------------------

void StaticMeshLoader::__ProcessAINode(aiNode* node, const aiScene* scene, Components::StaticMesh& out)
{
	constexpr auto vertex = Vertex<Position, Normal, TextureCoord, Tangent>{};
	constexpr auto stride = sizeof(vertex);
	constexpr auto offsetPosition = 0;
	constexpr auto offsetNormal = sizeof(Position);
	constexpr auto offsetTC = offsetNormal + sizeof(Normal);
	constexpr auto offsetTangent = offsetTC + sizeof(TextureCoord);
	
	constexpr auto vertexFormat0 = VertexFormat{ 3, VertexAttribType::FLOAT, false, static_cast<i32>(offsetPosition) };
	constexpr auto vertexFormat1 = VertexFormat{ 3, VertexAttribType::FLOAT, false, static_cast<i32>(offsetNormal) };
	constexpr auto vertexFormat2 = VertexFormat{ 2, VertexAttribType::FLOAT, false, static_cast<i32>(offsetTC) };
	constexpr auto vertexFormat3 = VertexFormat{ 3, VertexAttribType::FLOAT, false, static_cast<i32>(offsetTangent) };

	for (auto i = 0u; i < node->mNumMeshes; i++)
	{
		auto& mesh = out.meshArray[out.nrMeshes++];
		mesh.Create();
		mesh.SetupAttributeFloat(0, 0, vertexFormat0);
		mesh.SetupAttributeFloat(1, 0, vertexFormat1);
		mesh.SetupAttributeFloat(2, 0, vertexFormat2);
		mesh.SetupAttributeFloat(3, 0, vertexFormat3);
		
		auto aimesh = scene->mMeshes[node->mMeshes[i]];
		
		// Load vertices
		auto vbo = __LoadVertices(aimesh);
		mesh.vertexArray->AttachVertexBuffer(0, vbo, 0, stride);
		mesh.numVertices = aimesh->mNumVertices;
		// Load indices
		auto ebo = __LoadIndices(aimesh);
		mesh.vertexArray->AttachElementBuffer(ebo);
		mesh.numIndices = aimesh->mNumFaces * 3;
		
		if (scene->HasMaterials())
			__LoadMeshMaterial(scene, aimesh, mesh);
	}
		
	// Then do the same for each of its children
	for (auto i = 0u; i < node->mNumChildren; i++)
		__ProcessAINode(node->mChildren[i], scene, out);
}
Buffer StaticMeshLoader::__LoadVertices(aiMesh* aimesh)
{
	constexpr auto vertex = Vertex<Position, Normal, TextureCoord, Tangent>{};
	auto size = aimesh->mNumVertices * sizeof(vertex);

	auto buffer = Buffer{};
	buffer.Create();
	buffer.CreateStorage(size, nullptr, BufferUsage::STATIC_DRAW);

	auto ptr = static_cast<f32*>(buffer.MapStorage(BufferAccess::WRITE_ONLY));
	for (auto i = 0u; i < aimesh->mNumVertices; i++)
	{
		// Position
		*(ptr++) = static_cast<f32>(aimesh->mVertices[i].x);
		*(ptr++) = static_cast<f32>(aimesh->mVertices[i].y);
		*(ptr++) = static_cast<f32>(aimesh->mVertices[i].z);
		// Normal
		*(ptr++) = static_cast<f32>(aimesh->mNormals[i].x);
		*(ptr++) = static_cast<f32>(aimesh->mNormals[i].y);
		*(ptr++) = static_cast<f32>(aimesh->mNormals[i].z);
		// Texture coords
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
Buffer StaticMeshLoader::__LoadIndices(aiMesh* aimesh)
{
	auto numIndices = aimesh->mNumFaces * 3;
	auto size = numIndices * sizeof(u32);

	auto buffer = Buffer{};
	buffer.Create();
	buffer.CreateStorage(size, nullptr, BufferUsage::STATIC_DRAW);

	auto ptr = static_cast<u32 *>(buffer.MapStorage(BufferAccess::WRITE_ONLY));
	for (auto i = 0u; i < aimesh->mNumFaces; i++)
	{
		auto& face = aimesh->mFaces[i];
		for (auto j = 0u; j < face.mNumIndices; j++)
			*(ptr++) = static_cast<u32>(face.mIndices[j]);
	}
	buffer.UnmapStorage();
	return buffer;
}
void StaticMeshLoader::__LoadMeshMaterial(const aiScene* scene, aiMesh* aimesh, Mesh& mesh)
{
	auto& manager = TexturesManager::GetInstance();

	auto fileName = aiString{};
	auto aimaterial = scene->mMaterials[aimesh->mMaterialIndex];
	if (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &fileName) == aiReturn_SUCCESS)
		mesh.material.albedo = manager.GetOrCreateTexture(fileName.C_Str());

	if (aimaterial->GetTexture(aiTextureType_NORMALS, 0, &fileName) == aiReturn_SUCCESS)
		mesh.material.normalMap = manager.GetOrCreateTexture(fileName.C_Str());

	//if (aimaterial->GetTexture(aiTextureType_SPECULAR, 0, &fileName) == aiReturn_SUCCESS)
	//	mesh.material.specular = manager.GetOrCreateTexture(fileName.C_Str());
}
