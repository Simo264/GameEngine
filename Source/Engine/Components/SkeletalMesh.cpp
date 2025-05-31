#include "SkeletalMesh.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Material.hpp"
#include "Engine/Vertex.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"
#include "Engine/Managers/TexturesManager.hpp"
#include "Engine/Globals.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static Mat4f AiMatrixToGLM(const aiMatrix4x4 &matrix)
{
	Mat4f m{};
	// the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	m[0][0] = matrix.a1;
	m[1][0] = matrix.a2;
	m[2][0] = matrix.a3;
	m[3][0] = matrix.a4;
	m[0][1] = matrix.b1;
	m[1][1] = matrix.b2;
	m[2][1] = matrix.b3;
	m[3][1] = matrix.b4;
	m[0][2] = matrix.c1;
	m[1][2] = matrix.c2;
	m[2][2] = matrix.c3;
	m[3][2] = matrix.c4;
	m[0][3] = matrix.d1;
	m[1][3] = matrix.d2;
	m[2][3] = matrix.d3;
	m[3][3] = matrix.d4;
	return m;
}

// ----------------------------------------------------
//										PUBLIC
// ----------------------------------------------------

void SkeletalMesh::CreateFromFile(const fs::path &absolute)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(absolute.string().c_str(),
																					 aiProcess_Triangulate |
																							 aiProcess_GenUVCoords |
																							 aiProcess_FlipUVs |
																							 aiProcess_JoinIdenticalVertices |
																							 aiProcess_CalcTangentSpace |
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

	u32 totalBones = std::accumulate(scene->mMeshes, scene->mMeshes + scene->mNumMeshes, 0, [](u32 sum, aiMesh *mesh)
																	 { return sum + mesh->mNumBones; });
	assert(totalBones <= GetMaxNumBones());

	meshes = std::make_unique<Mesh[]>(scene->mNumMeshes);
	bones = std::make_shared<Bone[]>(totalBones);
	boneNames = std::make_shared<Array<char, 32>[]>(totalBones);
	rootNode = std::make_shared<BoneNode>();

	ProcessNode(scene->mRootNode, scene);
	LoadBoneHierarchy(*rootNode, scene->mRootNode);
}

void SkeletalMesh::Clone(SkeletalMesh &other) const
{
	other.nrMeshes = nrMeshes;
	other.meshes = std::make_unique<Mesh[]>(nrMeshes);
	for (u32 i = 0; i < nrMeshes; i++)
		other.meshes[i] = meshes[i];

	other.bones = bones;
	other.boneNames = boneNames;
	other.rootNode = rootNode;
	other.nrBones = nrBones;

	other.id = id;
}

void SkeletalMesh::Destroy() const
{
	for (u32 i = 0; i < nrMeshes; i++)
		meshes[i].Destroy();
}

void SkeletalMesh::Render(Program program, RenderMode mode) const
{
	for (u32 i = 0; i < nrMeshes; i++)
		meshes[i].Render(program, mode);
}

i32 SkeletalMesh::FindBone(StringView boneName) const
{
	assert(boneName.size() <= 32);

	for (u32 i = 0; i < nrBones; i++)
		if (boneName == boneNames[i].data())
			return i;

	return -1;
}

// ----------------------------------------------------
//										PRIVATE
// ----------------------------------------------------

void SkeletalMesh::ProcessNode(aiNode *node, const aiScene *scene)
{
	for (u32 i = 0; i < node->mNumMeshes; i++)
	{
		u32 MAX_BONES_INFLUENCE = Vertex_P_N_UV_T_B::MAX_BONES_INFLUENCE;

		Mesh &mesh = meshes[nrMeshes++];
		mesh.Create();
		mesh.SetupAttributeFloat(0, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, position)));
		mesh.SetupAttributeFloat(1, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, normal)));
		mesh.SetupAttributeFloat(2, 0, VertexFormat(2, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, uv)));
		mesh.SetupAttributeFloat(3, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, tangent)));
		mesh.SetupAttributeInteger(4, 0, VertexFormat(MAX_BONES_INFLUENCE, VertexAttribType::INT, false, offsetof(Vertex_P_N_UV_T_B, boneIds)));
		mesh.SetupAttributeFloat(5, 0, VertexFormat(MAX_BONES_INFLUENCE, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, boneWeights)));

		aiMesh *aimesh = scene->mMeshes[node->mMeshes[i]];

		// 1) Load vertex data
		Buffer vbo = LoadVertices(aimesh);
		mesh.vertexArray->AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_N_UV_T_B));
		mesh.numVertices = aimesh->mNumVertices;

		// 2) Load indices
		Buffer ebo = LoadIndices(aimesh);
		mesh.vertexArray->AttachElementBuffer(ebo);
		mesh.numIndices = aimesh->mNumFaces * 3;

		if (scene->HasMaterials())
		{
			aiString filename;
			aiMaterial *material = scene->mMaterials[aimesh->mMaterialIndex];
			auto &texManager = TexturesManager::GetInstance();
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &filename) == aiReturn_SUCCESS)
				mesh.material.diffuse = texManager.GetOrCreateTexture(filename.C_Str());
			if (material->GetTexture(aiTextureType_SPECULAR, 0, &filename) == aiReturn_SUCCESS)
				mesh.material.specular = texManager.GetOrCreateTexture(filename.C_Str());
			if (material->GetTexture(aiTextureType_NORMALS, 0, &filename) == aiReturn_SUCCESS)
				mesh.material.normal = texManager.GetOrCreateTexture(filename.C_Str());
		}
	}

	// Then do the same for each of its children
	for (u32 i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

Buffer SkeletalMesh::LoadVertices(aiMesh *aimesh)
{
	Vector<Vertex_P_N_UV_T_B> vertices;
	vertices.reserve(aimesh->mNumVertices);
	for (u32 i = 0; i < aimesh->mNumVertices; i++)
	{
		const auto &position = aimesh->mVertices[i];
		const auto &normal = aimesh->mNormals[i];
		const auto &uv = aimesh->mTextureCoords[0][i];
		const auto &tangent = aimesh->mTangents[i];

		Vertex_P_N_UV_T_B &vertex = vertices.emplace_back();
		vertex.position = Vec3F(position.x, position.y, position.z);
		if (aimesh->HasNormals())
			vertex.normal = Vec3F(normal.x, normal.y, normal.z);
		if (aimesh->HasTextureCoords(0))
			vertex.uv = Vec2F(uv.x, uv.y);
		if (aimesh->HasTangentsAndBitangents())
			vertex.tangent = Vec3F(tangent.x, tangent.y, tangent.z);
	}
	LoadBonesAndWeights(vertices, aimesh);

	Buffer buffer(vertices.size() * sizeof(Vertex_P_N_UV_T_B), vertices.data(), BufferUsage::STATIC_DRAW);
	return buffer;
}

Buffer SkeletalMesh::LoadIndices(aiMesh *aimesh)
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

void SkeletalMesh::LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B> &vertices, const aiMesh *aimesh)
{
	for (u32 i = 0; i < aimesh->mNumBones; i++)
	{
		aiBone *aibone = aimesh->mBones[i];
		StringView aiboneName = aibone->mName.C_Str();
		assert(aiboneName.size() < 32);

		i32 boneIndex{};

		i32 boneIdx = FindBone(aiboneName);
		if (boneIdx == -1)
		{
			Bone &bone = bones[nrBones];
			bone.offset = AiMatrixToGLM(aibone->mOffsetMatrix);
			Array<char, 32> &boneName = boneNames[nrBones];
			boneName.fill(0);
			std::copy(aiboneName.begin(), aiboneName.end(), boneName.begin());
			boneIndex = nrBones;
			nrBones++;
		}
		else
			boneIndex = boneIdx;

		for (u32 weightIndex = 0; weightIndex < aibone->mNumWeights; weightIndex++)
		{
			u32 vertexId = aibone->mWeights[weightIndex].mVertexId;
			f32 weight = aibone->mWeights[weightIndex].mWeight;
			Vertex_P_N_UV_T_B &vertex = vertices.at(vertexId);
			vertex.AddBone(boneIndex, weight);
		}
	}
}

void SkeletalMesh::LoadBoneHierarchy(BoneNode &dest, const aiNode *src)
{
	StringView aiboneName = src->mName.data;
	i32 boneIndex = FindBone(aiboneName);

	dest.bindPoseTransform = AiMatrixToGLM(src->mTransformation);
	dest.index = boneIndex;
	dest.children.reserve(src->mNumChildren);
	for (u32 i = 0; i < src->mNumChildren; i++)
	{
		BoneNode &child = dest.children.emplace_back();
		LoadBoneHierarchy(child, src->mChildren[i]);
	}
}
