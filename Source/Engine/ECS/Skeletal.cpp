#include "Skeletal.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static Buffer LoadVertices(aiMesh* aimesh, i32& numVertices)
{
	numVertices = aimesh->mNumVertices;
	u64 size = numVertices * sizeof(Vertex_P_N_UV_T_B);

	Buffer buffer;
	buffer.Create();
	buffer.CreateStorage(size, nullptr, GL_STATIC_DRAW);

	f32* ptr = static_cast<f32*>(buffer.MapStorage(GL_WRITE_ONLY));
	for (u32 i = 0; i < numVertices; i++)
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
		/* boneIDs */
		for (i32 j = 0; j < 4; j++)
			*(ptr++) = -1.f;
		/* boneWeights */
		for (i32 j = 0; j < 4; j++)
			*(ptr++) = 0.f;
	}
	buffer.UnmapStorage();
	return buffer;
}
static Buffer LoadIndices(aiMesh* aimesh, i32& numIndices)
{
	static auto countIndices = [](i32 n, aiFace& face) { return n + face.mNumIndices; };
	numIndices = std::reduce(
		aimesh->mFaces,
		aimesh->mFaces + aimesh->mNumFaces,
		0,
		countIndices
	);
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
static Texture2D* GetMaterialTexture(aiMaterial* material, aiTextureType type)
{
	aiString fileName;
	if (material->GetTexture(type, 0, &fileName) == aiReturn_SUCCESS)
		return &TextureManager::Get().GetTextureByPath(GetTexturesPath() / fileName.C_Str());

	return nullptr;
}
static mat4f ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
	mat4f to{};
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Skeletal::Skeletal(const fs::path& skeletalPath)
	: strPath{ skeletalPath.string() },
		meshes{},
		_boneCounter{ 0 }
{
	CONSOLE_TRACE("Loading skeletal {}...", strPath);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(strPath.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		CONSOLE_ERROR("Assimp importer error: {}", importer.GetErrorString());
		return;
	}

	meshes.reserve(scene->mNumMeshes);
	ProcessNode(scene->mRootNode, scene);

	CONSOLE_TRACE("Skeletal loaded!");
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void Skeletal::ProcessNode(aiNode* node, const aiScene* scene)
{
	/* Process all the node's meshes */
	for (i32 i = 0; i < node->mNumMeshes; i++)
	{
		Mesh& mesh = meshes.emplace_back();
		mesh.SetupAttributeFloat(0, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, position)));
		mesh.SetupAttributeFloat(1, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, normal)));
		mesh.SetupAttributeFloat(2, 0, VertexFormat(2, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, uv)));
		mesh.SetupAttributeFloat(3, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, tangent)));
		mesh.SetupAttributeInteger(4, 0, VertexFormat(MAX_BONE_WEIGHTS, GL_INT, false, offsetof(Vertex_P_N_UV_T_B, boneIDs)));
		mesh.SetupAttributeFloat(5, 0, VertexFormat(MAX_BONE_WEIGHTS, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, boneWeights)));

		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];

		/* 1) Load vertex data */
		Vector<Vertex_P_N_UV_T_B> vertices;
		vertices.reserve(aimesh->mNumVertices);
		for (u32 v = 0; v < aimesh->mNumVertices; v++)
		{
			Vertex_P_N_UV_T_B vertex;
			vertex.position = vec3f{ aimesh->mVertices[v].x, aimesh->mVertices[v].y, aimesh->mVertices[v].z };
			if (aimesh->HasNormals())
				vertex.normal = vec3f{ aimesh->mNormals[v].x, aimesh->mNormals[v].y, aimesh->mNormals[v].z };
			if (aimesh->HasTextureCoords(0))
				vertex.uv = vec2f{ aimesh->mTextureCoords[0][v].x, aimesh->mTextureCoords[0][v].y };
			if (aimesh->HasTangentsAndBitangents())
				vertex.tangent = vec3f{ aimesh->mTangents[v].x, aimesh->mTangents[v].y, aimesh->mTangents[v].z };
			
			vertices.push_back(vertex);
		}

		/* 2) Load indices */
		Vector<u32> indices;
		indices.reserve(static_cast<u64>(aimesh->mNumFaces * 3));
		for (u32 f = 0; f < aimesh->mNumFaces; f++)
		{
			aiFace face = aimesh->mFaces[f];
			for (u32 j = 0; j < face.mNumIndices; ++j)
				indices.push_back(face.mIndices[j]);
		}

		/* 3) Load bones and weights */
		for (i32 boneIndex = 0; boneIndex < aimesh->mNumBones; boneIndex++)
		{
			i32 boneID = -1;
			String boneName = aimesh->mBones[boneIndex]->mName.C_Str();

			if (_boneInfoMap.find(boneName) == _boneInfoMap.end())
			{
				BoneInfo newBoneInfo{};
				newBoneInfo.id = _boneCounter;
				newBoneInfo.offset = ConvertMatrixToGLMFormat(aimesh->mBones[boneIndex]->mOffsetMatrix);
				
				_boneInfoMap[boneName] = newBoneInfo;
				boneID = _boneCounter++;
			}
			else
			{
				boneID = _boneInfoMap[boneName].id;
			}
			assert(boneID != -1);

			aiVertexWeight* weights = aimesh->mBones[boneIndex]->mWeights;
			u32 numWeights = aimesh->mBones[boneIndex]->mNumWeights;
			for (i32 weightIndex = 0; weightIndex < numWeights; weightIndex++)
			{
				i32 vertexId = weights[weightIndex].mVertexId;
				f32 weight = weights[weightIndex].mWeight;
				assert(vertexId <= vertices.size());
				
				Vertex_P_N_UV_T_B& vertex = vertices[vertexId];
				u32 nBones = std::count_if(std::begin(vertex.boneIDs), std::end(vertex.boneIDs), [](i32 id) { return id != -1; });
				vertex.boneIDs[nBones] = boneID;
				vertex.boneWeights[nBones] = weight;
			}
		}

	
		Buffer vbo(vertices.size() * sizeof(Vertex_P_N_UV_T_B), vertices.data(), GL_STATIC_DRAW);
		Buffer ebo(indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
		mesh.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_N_UV_T_B));
		mesh.vao.AttachElementBuffer(ebo);
	}

	/* Then do the same for each of its children */
	for (i32 i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}
