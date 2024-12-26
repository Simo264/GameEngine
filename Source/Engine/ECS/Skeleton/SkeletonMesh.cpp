#include "SkeletonMesh.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

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

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

SkeletonMesh::SkeletonMesh(const fs::path& skeletalPath) :
	path{ skeletalPath },
	meshes{},
	_boneMap{}
{
	CONSOLE_INFO("Loading skeleton mesh {}...", path.string());

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string().c_str(),
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

	meshes.reserve(scene->mNumMeshes);
	ProcessNode(scene->mRootNode, scene);
	LoadBoneHierarchy(rootNode, scene->mRootNode);
}
void SkeletonMesh::Draw(i32 mode)
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
std::pair<Bone*, u32> SkeletonMesh::FindBone(StringView boneName)
{
	auto it = _boneMap.find(boneName.data());
	if (it != _boneMap.end())
	{
		u32 boneIndex = it->second;
		return { &bones.at(boneIndex), boneIndex };
	}
	return { nullptr, -1 };
}
std::pair<Bone*, u32> SkeletonMesh::InsertBone(StringView boneName)
{
	u32 boneIndex = bones.size();
	
	auto [it, success] = _boneMap.insert({ boneName.data(), boneIndex });
	if (success)
	{
		Bone& bone = bones.emplace_back();
		return { &bone, boneIndex };
	}
	
	CONSOLE_WARN("Can't insert bone '{}'", boneName.data());
	return { nullptr, -1 };
}
Bone* SkeletonMesh::GetBone(u32 i)
{
	if (i < 0 || i >= bones.size())
		return nullptr;
	return &bones.at(i);
}
u32 SkeletonMesh::TotalVertices() const
{
	return std::reduce(meshes.begin(), meshes.end(), 0, [](i32 acc, const Mesh& mesh) {
		return acc + mesh.vao.numVertices;
	});
}
u32 SkeletonMesh::TotalIndices() const
{
	return std::reduce(meshes.begin(), meshes.end(), 0, [](i32 acc, const Mesh& mesh) {
		return acc + mesh.vao.numIndices;
	});
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void SkeletonMesh::ProcessNode(aiNode* node, const aiScene* scene)
{
	/* Process all the node's meshes */
	for (i32 i = 0; i < node->mNumMeshes; i++)
	{
		u32 MAX_BONES_INFLUENCE = Vertex_P_N_UV_T_B::MAX_BONES_INFLUENCE;

		Mesh& mesh = meshes.emplace_back();
		mesh.SetupAttributeFloat(0, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, position)));
		mesh.SetupAttributeFloat(1, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, normal)));
		mesh.SetupAttributeFloat(2, 0, VertexFormat(2, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, uv)));
		mesh.SetupAttributeFloat(3, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, tangent)));
		mesh.SetupAttributeInteger(4, 0, VertexFormat(MAX_BONES_INFLUENCE, GL_INT, false, offsetof(Vertex_P_N_UV_T_B, boneIds)));
		mesh.SetupAttributeFloat(5, 0, VertexFormat(MAX_BONES_INFLUENCE, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T_B, boneWeights)));

		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];

		/* 1) Load vertex data */
		Buffer vbo = LoadVertices(aimesh);
		mesh.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_N_UV_T_B));
		mesh.vao.numVertices = aimesh->mNumVertices;

		/* 2) Load indices */
		Buffer ebo = LoadIndices(aimesh);
		mesh.vao.AttachElementBuffer(ebo);
		mesh.vao.numIndices = aimesh->mNumFaces * 3;

		if (scene->HasMaterials())
		{
			aiString filename;
			aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &filename) == aiReturn_SUCCESS)
			{
				auto& diffuse = TextureManager::Get().GetTextureByPath(GetTexturesPath() / filename.C_Str());
				mesh.material.diffuse = &diffuse;
			}
			if (material->GetTexture(aiTextureType_SPECULAR, 0, &filename) == aiReturn_SUCCESS)
			{
				auto& specular = TextureManager::Get().GetTextureByPath(GetTexturesPath() / filename.C_Str());
				mesh.material.specular = &specular;
			}
			if (material->GetTexture(aiTextureType_NORMALS, 0, &filename) == aiReturn_SUCCESS)
			{
				auto& normal = TextureManager::Get().GetTextureByPath(GetTexturesPath() / filename.C_Str());
				mesh.material.normal = &normal;
			}
		}
	}

	/* Then do the same for each of its children */
	for (i32 i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}
Buffer SkeletonMesh::LoadVertices(aiMesh* aimesh)
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

	Buffer buffer(vertices.size() * sizeof(Vertex_P_N_UV_T_B), vertices.data(), GL_STATIC_DRAW);
	return buffer;
}
void SkeletonMesh::LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh)
{
	for (u32 i = 0; i < aimesh->mNumBones; i++)
	{
		aiBone* aibone = aimesh->mBones[i];
		const char* boneName = aibone->mName.C_Str();

		u32 boneIndex = -1;
		auto [bone, index] = FindBone(boneName);
		if (!bone)
		{
			auto [newBone, newIndex] = InsertBone(boneName);
			newBone->offset = AiMatrixToGLM(aibone->mOffsetMatrix);
			boneIndex = newIndex;
		}
		else
			boneIndex = index;

		for (u32 weightIndex = 0; weightIndex < aibone->mNumWeights; weightIndex++)
		{
			u32 vertexId = aibone->mWeights[weightIndex].mVertexId;
			f32 weight = aibone->mWeights[weightIndex].mWeight;
			Vertex_P_N_UV_T_B& vertex = vertices.at(vertexId);
			vertex.AddBone(boneIndex, weight);
		}
	}
}
Buffer SkeletonMesh::LoadIndices(aiMesh* aimesh)
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
void SkeletonMesh::LoadBoneHierarchy(BoneNode& dest, const aiNode* src)
{
	auto [bone, index] = FindBone(src->mName.data);
	dest.boneIndex = index;
	dest.transformation = AiMatrixToGLM(src->mTransformation);
	dest.children.reserve(src->mNumChildren);
	for (u32 i = 0; i < src->mNumChildren; i++)
	{
		BoneNode& child = dest.children.emplace_back();
		LoadBoneHierarchy(child, src->mChildren[i]);
	}
}
Texture2D* SkeletonMesh::GetMaterialTexture(aiMaterial* material, u32 textureType)
{
	aiString fileName;
	if (material->GetTexture(static_cast<aiTextureType>(textureType), 0, &fileName) == aiReturn_SUCCESS)
		return &TextureManager::Get().GetTextureByPath(GetTexturesPath() / fileName.C_Str());

	return nullptr;
}
