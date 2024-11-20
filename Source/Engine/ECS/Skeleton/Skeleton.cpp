#include "Skeleton.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static void LoadVertices(Vector<Vertex_P_N_UV_T_B>& vertices, aiMesh* aimesh)
{
	vertices.reserve(aimesh->mNumVertices);
	for (u32 i = 0; i < aimesh->mNumVertices; i++)
	{
		const auto& position = aimesh->mVertices[i];
		const auto& normal = aimesh->mNormals[i];
		const auto& uv = aimesh->mTextureCoords[0][i];
		const auto& tangent = aimesh->mTangents[i];

		Vertex_P_N_UV_T_B vertex;
		vertex.position = vec3f(position.x, position.y, position.z);
		if (aimesh->HasNormals())
			vertex.normal = vec3f(normal.x, normal.y, normal.z);
		if (aimesh->HasTextureCoords(0))
			vertex.uv = vec2f(uv.x, uv.y);
		if (aimesh->HasTangentsAndBitangents())
			vertex.tangent = vec3f(tangent.x, tangent.y, tangent.z);

		vertices.push_back(vertex);
	}
}
static void LoadIndices(Vector<u32>& indices, aiMesh* aimesh)
{
	indices.reserve(static_cast<u64>(aimesh->mNumFaces * 3));
	for (u32 i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];
		for (u32 j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}
}

static Texture2D* GetMaterialTexture(aiMaterial* material, aiTextureType type)
{
	aiString fileName;
	if (material->GetTexture(type, 0, &fileName) == aiReturn_SUCCESS)
		return &TextureManager::Get().GetTextureByPath(GetTexturesPath() / fileName.C_Str());

	return nullptr;
}
static mat4f AiMatrixToGLM(const aiMatrix4x4& matrix)
{
	mat4f m{};
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	m[0][0] = matrix.a1; m[1][0] = matrix.a2; m[2][0] = matrix.a3; m[3][0] = matrix.a4;
	m[0][1] = matrix.b1; m[1][1] = matrix.b2; m[2][1] = matrix.b3; m[3][1] = matrix.b4;
	m[0][2] = matrix.c1; m[1][2] = matrix.c2; m[2][2] = matrix.c3; m[3][2] = matrix.c4;
	m[0][3] = matrix.d1; m[1][3] = matrix.d2; m[2][3] = matrix.d3; m[3][3] = matrix.d4;
	return m;
}


/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Skeleton::Skeleton(const fs::path& skeletalPath)
	: strPath{ skeletalPath.string() },
		meshes{},
		_boneCounter{ 0 }
{
	CONSOLE_TRACE("Loading skeletal {}...", strPath);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(strPath.c_str(), 
		aiProcess_Triangulate | 
		aiProcess_GenSmoothNormals | 
		aiProcess_CalcTangentSpace
	);

	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		CONSOLE_ERROR("Assimp importer error: {}", importer.GetErrorString());
		return;
	}

	meshes.reserve(scene->mNumMeshes);
	ProcessNode(scene->mRootNode, scene);

	CONSOLE_TRACE("Skeletal loaded!");
}
void Skeleton::DrawSkeleton(i32 mode)
{
	for (auto& mesh : meshes)
	{
		auto& material = mesh.material;
		material.diffuse->BindTextureUnit(0);
		material.specular->BindTextureUnit(1);
		material.normal->BindTextureUnit(2);
		material.height->BindTextureUnit(3);
		mesh.Draw(mode);
	}

	glBindTextureUnit(0, 0);
	glBindTextureUnit(1, 0);
	glBindTextureUnit(2, 0);
	glBindTextureUnit(3, 0);
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void Skeleton::ProcessNode(aiNode* node, const aiScene* scene)
{
	/* Process all the node's meshes */
	for (i32 i = 0; i < node->mNumMeshes; i++)
	{
		Mesh& mesh = meshes.emplace_back();
		mesh.SetupAttributeFloat(0, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, position)));
		mesh.SetupAttributeFloat(1, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, normal)));
		mesh.SetupAttributeFloat(2, 0, VertexFormat(2, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, uv)));
		mesh.SetupAttributeFloat(3, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, tangent)));
		mesh.SetupAttributeInteger(4, 0, VertexFormat(MAX_BONE_INFLUENCE, GL_INT, false, offsetof(Vertex_P_N_UV_T_B, boneIDs)));
		mesh.SetupAttributeFloat(5, 0, VertexFormat(MAX_BONE_INFLUENCE, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, boneWeights)));

		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];

		/* 1) Load vertex data */
		Vector<Vertex_P_N_UV_T_B> vertices;
		LoadVertices(vertices, aimesh);

		/* 2) Load indices */
		Vector<u32> indices;
		LoadIndices(indices, aimesh);

		/* 3) Load bones and weights */
		LoadBonesAndWeights(vertices, aimesh);

		Buffer vbo(vertices.size() * sizeof(Vertex_P_N_UV_T_B), vertices.data(), GL_STATIC_DRAW);
		Buffer ebo(indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
		mesh.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_N_UV_T_B));
		mesh.vao.AttachElementBuffer(ebo);
		mesh.vao.numIndices = aimesh->mNumFaces * 3;
		mesh.vao.numVertices = aimesh->mNumVertices;
	}

	/* Then do the same for each of its children */
	for (i32 i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

void Skeleton::LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh)
{
	auto& boneInfoMap = _boneInfoMap;
	u32& boneCount = _boneCounter;

	for (u32 boneIndex = 0; boneIndex < aimesh->mNumBones; boneIndex++)
	{
		i32 boneID = -1;
		const char* boneName = aimesh->mBones[boneIndex]->mName.C_Str();
		auto it = boneInfoMap.find(boneName);
		if (it == boneInfoMap.end())
		{
			mat4f offset = AiMatrixToGLM(aimesh->mBones[boneIndex]->mOffsetMatrix);
			boneInfoMap.emplace(boneName, BoneInfo(boneCount, offset));
			boneID = boneCount;
			boneCount++;
		}
		else
		{
			boneID = it->second.id;
		}

		aiVertexWeight* weights = aimesh->mBones[boneIndex]->mWeights;
		u32 numWeights = aimesh->mBones[boneIndex]->mNumWeights;
		for (u32 weightIndex = 0; weightIndex < numWeights; weightIndex++)
		{
			u32 vertexId = weights[weightIndex].mVertexId;
			f32 weight = weights[weightIndex].mWeight;
			
			Vertex_P_N_UV_T_B& vertex = vertices.at(vertexId);
			i32 i = std::count_if(std::begin(vertex.boneIDs), std::end(vertex.boneIDs), [](i32 id) { return id != -1; });
			vertex.boneWeights[i] = weight;
			vertex.boneIDs[i] = boneID;
		}
	}
}