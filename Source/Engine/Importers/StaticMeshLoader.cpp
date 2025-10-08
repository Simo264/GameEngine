#include "StaticMeshLoader.hpp"
#include "Utils/Logger.hpp"

#include "Engine/ECS/Components/StaticMesh.hpp"
#include "Engine/ECS/Components/Material.hpp"
#include "Engine/Managers/TexturesManager.hpp"

#include <glad/gl.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void StaticMeshLoader::LoadDataFromFile(const fs::path& absolutePathToFile, 
																				Components::StaticMesh& meshDest,
																				Components::Material& materialDest)
{
	if (!meshDest.vertexArray.Valid())
	{
		CONSOLE_WARN("Static mesh must be a valid object");
		return;
	}

	constexpr auto flags = aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_GenUVCoords | 
		aiProcess_CalcTangentSpace |
		aiProcess_ImproveCacheLocality;
	
	auto importer = Assimp::Importer{};
	auto scene = importer.ReadFile(absolutePathToFile.string().c_str(), flags);
	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		CONSOLE_ERROR("Assimp importer error: {}", importer.GetErrorString());
		return;
	}
	if (scene->mNumMeshes == 0)
	{
		CONSOLE_ERROR("No meshes found in file.");
		return;
	}
	if (scene->mNumMeshes > 1)
		CONSOLE_WARN("File contains more than one mesh, only the first one will be loaded.");
	
	using Vertex = Components::StaticMesh::Vertex;
	constexpr auto stride = sizeof(Vertex);

	auto aimesh = scene->mMeshes[0];
	auto nrVertices = aimesh->mNumVertices;
	auto size = stride * nrVertices;

	// Load vertices
	auto& vertBuffer = meshDest.vertexBuffer;
	vertBuffer.CreateImmutableStorage(size, nullptr, BufferStorageFlags::MAP_WRITE);
	__LoadVertices<Vertex>(vertBuffer, nrVertices, aimesh);
	
	// Load indices
	auto nrIndices = static_cast<u64>(aimesh->mNumFaces * 3);
	size = nrIndices * sizeof(u32);
	auto& indexBuffer = meshDest.indexBuffer;
	indexBuffer.CreateImmutableStorage(size, nullptr, BufferStorageFlags::MAP_WRITE);
	__LoadIndices(indexBuffer, aimesh);

	meshDest.vertexArray.AttachVertexBuffer(0, vertBuffer.id, 0, stride);
	meshDest.vertexArray.AttachIndexBuffer(indexBuffer.id);
	meshDest.numIndices = nrIndices;
	meshDest.numVertices = nrVertices;

	// Load material
	for (auto i = 0u; i < scene->mNumMaterials; ++i)
	{
		auto& instance = TexturesManager::GetInstance();
		auto fileName = aiString{};

		auto aimaterial = scene->mMaterials[i];
		if (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &fileName) == aiReturn_SUCCESS)
			materialDest.albedo = instance.GetOrCreateTexture(fileName.C_Str());
		
		fileName = aiString{};
		if (aimaterial->GetTexture(aiTextureType_NORMALS, 0, &fileName) == aiReturn_SUCCESS)
			materialDest.normalMap = instance.GetOrCreateTexture(fileName.C_Str());
	}
}

template<typename Vertex>
void StaticMeshLoader::__LoadVertices(Buffer& vertBuffer, u32 nrVertices, aiMesh* aimesh)
{
	auto pVertData = vertBuffer.MapStorage<Vertex>(BufferAccess::WRITE_ONLY);
	for (auto i = 0u; i < nrVertices; i++)
	{
		pVertData[i] = Vertex{};
		pVertData[i].position.data.x = aimesh->mVertices[i].x;
		pVertData[i].position.data.y = aimesh->mVertices[i].y;
		pVertData[i].position.data.z = aimesh->mVertices[i].z;
		if (aimesh->mNormals)
		{
			pVertData[i].normal.data.x = aimesh->mNormals[i].x;
			pVertData[i].normal.data.y = aimesh->mNormals[i].y;
			pVertData[i].normal.data.z = aimesh->mNormals[i].z;
		}
		else
			CONSOLE_WARN("aimesh does't have mNormals");

		if (aimesh->mTextureCoords[0])
		{
			pVertData[i].tc.data.x = aimesh->mTextureCoords[0][i].x;
			pVertData[i].tc.data.y = aimesh->mTextureCoords[0][i].y;
		}
		else
			CONSOLE_WARN("aimesh does't have mTextureCoords");

		if (aimesh->mTangents)
		{
			pVertData[i].tangent.data.x = aimesh->mTangents[i].x;
			pVertData[i].tangent.data.y = aimesh->mTangents[i].y;
			pVertData[i].tangent.data.z = aimesh->mTangents[i].z;
		}
		else
			CONSOLE_WARN("aimesh does't have mTangents");
	}
	vertBuffer.UnmapStorage();
}

void StaticMeshLoader::__LoadIndices(Buffer& indexBuffer, aiMesh* aimesh)
{
	auto pIdxData = indexBuffer.MapStorage<u32>(BufferAccess::WRITE_ONLY);
	for (auto i = 0u; i < aimesh->mNumFaces; i++)
	{
		auto& face = aimesh->mFaces[i];
		pIdxData[i * 3 + 0] = face.mIndices[0];
		pIdxData[i * 3 + 1] = face.mIndices[1];
		pIdxData[i * 3 + 2] = face.mIndices[2];
	}
	indexBuffer.UnmapStorage();
}
