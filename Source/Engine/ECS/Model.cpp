#include "Model.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static u32 totalVertices = 0;
static u32 totalIndices = 0;
static u32 totalSize = 0;
static chrono::steady_clock::time_point timeStart;
static chrono::steady_clock::time_point timeEnd;

static Buffer LoadVertices(aiMesh* aimesh, i32& numVertices)
{
	numVertices = aimesh->mNumVertices;
	u64 size = numVertices * sizeof(Vertex_P_N_UV_T);

	totalVertices += numVertices;
	totalSize += size;

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

	totalIndices += numIndices;
	totalSize += size;

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

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Model::Model(const fs::path& modelPath)
	: strPath{ modelPath.string() },
		meshes{}
{
	CONSOLE_TRACE("Loading model {}...", strPath);
	timeStart = chrono::high_resolution_clock::now();

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

	totalVertices = 0;
	totalIndices = 0;
	totalSize = 0;
	meshes.reserve(scene->mNumMeshes);
	ProcessNode(scene->mRootNode, scene);

	timeEnd = chrono::high_resolution_clock::now();
	chrono::duration<f64> diff = timeEnd - timeStart;
	CONSOLE_TRACE("Time: {}s", diff.count());
	CONSOLE_TRACE("Nr. meshes: {}", scene->mNumMeshes);
	CONSOLE_TRACE("Vertices: {} ", totalVertices);
	CONSOLE_TRACE("Indices: {}", totalIndices);
	CONSOLE_TRACE("Size: {} bytes", totalSize);
}
void Model::DestroyModel()
{
	/* Destroy all meshes */
	for (auto& mesh : meshes)
		mesh.Destroy();

	/* Destroy Vector */
	Vector<Mesh>().swap(meshes);
	CONSOLE_TRACE("Model destroyed");
}
void Model::DrawModel(i32 mode)
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

void Model::ProcessNode(aiNode* node, const aiScene* scene)
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

		/* Load data */
		i32 numVertices = 0;
		i32 numIndices = 0;

		Buffer vbo = LoadVertices(aimesh, numVertices);
		mesh.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_N_UV_T));
		mesh.vao.numVertices = numVertices;
		
		Buffer ebo = LoadIndices(aimesh, numIndices);
		mesh.vao.AttachElementBuffer(ebo);
		mesh.vao.numIndices = numIndices;

		if (aimesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
			if (Texture2D* diffuse = GetMaterialTexture(material, aiTextureType_DIFFUSE))
				mesh.material.diffuse = diffuse;
			if (Texture2D* specular = GetMaterialTexture(material, aiTextureType_SPECULAR))
				mesh.material.specular = specular;
			if (Texture2D* normal = GetMaterialTexture(material, aiTextureType_NORMALS))
				mesh.material.normal = normal;
			if (Texture2D* height = GetMaterialTexture(material, aiTextureType_HEIGHT))
				mesh.material.height = height;
		}
	}

	/* Then do the same for each of its children */
	for (i32 i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

