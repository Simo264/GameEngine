#include "SkeletalMesh.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Graphics/Material.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Globals.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

// ----------------------------------------------------
//										PUBLIC													
// ----------------------------------------------------



void SkeletalMesh::CreateFromFile(const fs::path& absolute)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(absolute.string().c_str(),
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
		aiProcess_OptimizeMeshes
	);

	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		CONSOLE_ERROR("Assimp importer error: {}", importer.GetErrorString());
		return;
	}

	u32 totalBones = std::accumulate(scene->mMeshes, scene->mMeshes + scene->mNumMeshes, 0, [](u32 sum, aiMesh* mesh) {
		return sum + mesh->mNumBones;
	});
	assert(totalBones <= GetMaxNumBones());
	
	meshes = std::make_unique<Mesh[]>(scene->mNumMeshes);
	bones = std::make_unique<Bone[]>(totalBones);
	boneNames = std::make_unique<BoneName[]>(totalBones);
	rootNode = std::make_unique<BoneNode>();

	ProcessNode(scene->mRootNode, scene);
	LoadBoneHierarchy(*rootNode, scene->mRootNode);
}

void SkeletalMesh::Clone(SkeletalMesh& other) const
{
	other.nrMeshes = nrMeshes;
	other.meshes = std::make_unique<Mesh[]>(nrMeshes);
	for (u32 i = 0; i < nrMeshes; i++)
		other.meshes[i] = meshes[i];

	other.nrBones = nrBones;
	other.bones = std::make_unique<Bone[]>(nrBones);
	other.boneNames = std::make_unique<BoneName[]>(nrBones);
	for (u32 i = 0; i < nrBones; i++)
	{
		other.bones[i] = bones[i];
		other.boneNames[i] = boneNames[i];
	}

	other.rootNode = std::make_unique<BoneNode>();
	*other.rootNode = *rootNode;

	other.id = id;
}

void SkeletalMesh::Destroy() const
{
	for (u32 i = 0; i < nrMeshes; i++)
		meshes[i].Destroy();
}

void SkeletalMesh::Draw(RenderMode mode) const
{
	for (u32 i = 0; i < nrMeshes; i++)
	{
		auto& mesh = meshes[i];
		mesh.material.diffuse.BindTextureUnit(0);
		mesh.material.specular.BindTextureUnit(1);
		mesh.material.normal.BindTextureUnit(2);
		mesh.Draw(mode);
	}
}

std::pair<i32, Bone*> SkeletalMesh::FindBone(StringView boneName) const
{
	for (u32 i = 0; i < nrBones; i++)
		if (boneName == boneNames[i].name)
			return { i, &bones[i] };

	return { -1, nullptr };
}

u32 SkeletalMesh::TotalVertices() const
{
	return std::reduce(meshes.get(), meshes.get() + nrMeshes, 0, [](i32 acc, const Mesh& mesh) {
		return acc + mesh.vao.numVertices;
	});
}

u32 SkeletalMesh::TotalIndices() const
{
	return std::reduce(meshes.get(), meshes.get() + nrMeshes, 0, [](i32 acc, const Mesh& mesh) {
		return acc + mesh.vao.numIndices;
	});
}

// ----------------------------------------------------
//										PRIVATE													
// ----------------------------------------------------

void SkeletalMesh::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (i32 i = 0; i < node->mNumMeshes; i++)
	{
		u32 MAX_BONES_INFLUENCE = Vertex_P_N_UV_T_B::MAX_BONES_INFLUENCE;

		Mesh& mesh = meshes[nrMeshes++];
		mesh.Create();
		mesh.SetupAttributeFloat(0, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, position)));
		mesh.SetupAttributeFloat(1, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, normal)));
		mesh.SetupAttributeFloat(2, 0, VertexFormat(2, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, uv)));
		mesh.SetupAttributeFloat(3, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, tangent)));
		mesh.SetupAttributeInteger(4, 0, VertexFormat(MAX_BONES_INFLUENCE, VertexAttribType::INT, false, offsetof(Vertex_P_N_UV_T_B, boneIds)));
		mesh.SetupAttributeFloat(5, 0, VertexFormat(MAX_BONES_INFLUENCE, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T_B, boneWeights)));

		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];

		// 1) Load vertex data
		Buffer vbo = LoadVertices(aimesh);
		mesh.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_N_UV_T_B));
		mesh.vao.numVertices = aimesh->mNumVertices;

		// 2) Load indices
		Buffer ebo = LoadIndices(aimesh);
		mesh.vao.AttachElementBuffer(ebo);
		mesh.vao.numIndices = aimesh->mNumFaces * 3;

		if (scene->HasMaterials())
		{
			aiString filename;
			aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
			auto& texManager = TexturesManager::Get();
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &filename) == aiReturn_SUCCESS)
				mesh.material.diffuse = texManager.GetOrCreateTexture(filename.C_Str());
			if (material->GetTexture(aiTextureType_SPECULAR, 0, &filename) == aiReturn_SUCCESS)
				mesh.material.specular = texManager.GetOrCreateTexture(filename.C_Str());
			if (material->GetTexture(aiTextureType_NORMALS, 0, &filename) == aiReturn_SUCCESS)
				mesh.material.normal = texManager.GetOrCreateTexture(filename.C_Str());
		}
	}

	// Then do the same for each of its children
	for (i32 i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

Buffer SkeletalMesh::LoadVertices(aiMesh* aimesh)
{
	Vector<Vertex_P_N_UV_T_B> vertices;
	vertices.reserve(aimesh->mNumVertices);
	for (u32 i = 0; i < aimesh->mNumVertices; i++)
	{
		const auto& position = aimesh->mVertices[i];
		const auto& normal = aimesh->mNormals[i];
		const auto& uv = aimesh->mTextureCoords[0][i];
		const auto& tangent = aimesh->mTangents[i];

		Vertex_P_N_UV_T_B& vertex = vertices.emplace_back();
		vertex.position = vec3f(position.x, position.y, position.z);
		if (aimesh->HasNormals())
			vertex.normal = vec3f(normal.x, normal.y, normal.z);
		if (aimesh->HasTextureCoords(0))
			vertex.uv = vec2f(uv.x, uv.y);
		if (aimesh->HasTangentsAndBitangents())
			vertex.tangent = vec3f(tangent.x, tangent.y, tangent.z);
	}
	LoadBonesAndWeights(vertices, aimesh);

	Buffer buffer(vertices.size() * sizeof(Vertex_P_N_UV_T_B), vertices.data(), BufferUsage::STATIC_DRAW);
	return buffer;
}

Buffer SkeletalMesh::LoadIndices(aiMesh* aimesh)
{
	u32 numIndices = aimesh->mNumFaces * 3;
	u64 size = numIndices * sizeof(u32);

	Buffer buffer;
	buffer.Create();
	buffer.CreateStorage(size, nullptr, BufferUsage::STATIC_DRAW);

	u32* ptr = static_cast<u32*>(buffer.MapStorage(BufferAccess::WRITE_ONLY));
	for (u32 i = 0; i < aimesh->mNumFaces; i++)
	{
		const aiFace& face = aimesh->mFaces[i];
		for (i32 i = 0; i < face.mNumIndices; i++)
			*(ptr++) = static_cast<u32>(face.mIndices[i]);
	}
	buffer.UnmapStorage();
	return buffer;
}

void SkeletalMesh::LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh)
{
	for (u32 i = 0; i < aimesh->mNumBones; i++)
	{
		aiBone* aibone = aimesh->mBones[i];
		const char* aiboneName = aibone->mName.C_Str();

		i32 boneIndex{};
		
		auto [boneIdx, bone] = FindBone(aiboneName);
		if (!bone)
		{
			auto& bone = bones[nrBones];
			bone.offset = AiMatrixToGLM(aibone->mOffsetMatrix);
			BoneName& boneName = boneNames[nrBones];
			
			assert(std::strlen(aiboneName) <= sizeof(boneName.name));
			std::strncpy(boneName.name, aiboneName, sizeof(boneName.name));
			boneIndex = nrBones;
			nrBones++;
		}
		else
			boneIndex = boneIdx;

		for (u32 weightIndex = 0; weightIndex < aibone->mNumWeights; weightIndex++)
		{
			u32 vertexId = aibone->mWeights[weightIndex].mVertexId;
			f32 weight = aibone->mWeights[weightIndex].mWeight;
			Vertex_P_N_UV_T_B& vertex = vertices.at(vertexId);
			vertex.AddBone(boneIndex, weight);
		}
	}
}

void SkeletalMesh::LoadBoneHierarchy(BoneNode& dest, const aiNode* src)
{
	const char* aiboneName = src->mName.data;
	auto [index, bone] = FindBone(aiboneName);
	
	dest.bindPoseTransform = AiMatrixToGLM(src->mTransformation);
	dest.index = index;
	dest.children.reserve(src->mNumChildren);
	for (u32 i = 0; i < src->mNumChildren; i++)
	{
		BoneNode& child = dest.children.emplace_back();
		LoadBoneHierarchy(child, src->mChildren[i]);
	}
}
