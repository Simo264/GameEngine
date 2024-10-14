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

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Model::Model(StringView modelPath)
	: strPath{ modelPath.data() },
		meshes{}
{
	CONSOLE_TRACE("Loading model {}...", strPath);
	timeStart = chrono::high_resolution_clock::now();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath.data(),
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
	CONSOLE_TRACE("time: {}s", diff.count());
	CONSOLE_TRACE("numMeshes: {}", scene->mNumMeshes);
	CONSOLE_TRACE("totalVertices: {} ", totalVertices);
	CONSOLE_TRACE("totalIndices: {}", totalIndices);
	CONSOLE_TRACE("totalSize: {} bytes", totalSize);
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
		mesh.SetupAttribute(0, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T, position)));
		mesh.SetupAttribute(1, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T, normal)));
		mesh.SetupAttribute(2, 0, VertexFormat(2, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T, uv))); 
		mesh.SetupAttribute(3, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_N_UV_T, tangent)));

		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];

		/* Load vertex buffer */
		i32 numVertices = aimesh->mNumVertices;
		u64 vSize = numVertices * sizeof(Vertex_P_N_UV_T);
		Buffer vbo(vSize, nullptr, GL_STATIC_DRAW);
		LoadVertices(aimesh, vbo);
		mesh.vao.AttachVertexBuffer(0, vbo.id, 0, sizeof(Vertex_P_N_UV_T));

		/* Load index buffer */
		static auto countIndices = [](i32 n, aiFace& face) { return n + face.mNumIndices; };
		i32 numIndices = std::reduce(
			aimesh->mFaces,
			aimesh->mFaces + aimesh->mNumFaces, 
			0, 
			countIndices
		);
		u64 iSize = numIndices * sizeof(u32);
		Buffer ebo(iSize, nullptr, GL_STATIC_DRAW);
		LoadIndices(aimesh, ebo);
		mesh.vao.AttachElementBuffer(ebo.id);
		mesh.vao.numVertices = numVertices;
		mesh.vao.numIndices = numIndices;
		totalVertices += numVertices;
		totalIndices += numIndices;
		totalSize += vSize + iSize;

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
void Model::LoadVertices(aiMesh* aimesh, Buffer& vbo)
{
	f32* vboPtr = static_cast<f32*>(vbo.MapStorage(GL_WRITE_ONLY));
	if (!vboPtr)
	{
		CONSOLE_ERROR("Error on mapping vertex buffer storage");
		return;
	}

	for (u32 i = 0; i < aimesh->mNumVertices; i++)
	{
		/* Position */
		*(vboPtr++) = static_cast<f32>(aimesh->mVertices[i].x);
		*(vboPtr++) = static_cast<f32>(aimesh->mVertices[i].y);
		*(vboPtr++) = static_cast<f32>(aimesh->mVertices[i].z);
		/* Normal */
		*(vboPtr++) = static_cast<f32>(aimesh->mNormals[i].x);
		*(vboPtr++) = static_cast<f32>(aimesh->mNormals[i].y);
		*(vboPtr++) = static_cast<f32>(aimesh->mNormals[i].z);
		/* Uv coordinates */
		*(vboPtr++) = static_cast<f32>(aimesh->mTextureCoords[0][i].x);
		*(vboPtr++) = static_cast<f32>(aimesh->mTextureCoords[0][i].y);
		/* Tangent */
		*(vboPtr++) = static_cast<f32>(aimesh->mTangents[i].x);
		*(vboPtr++) = static_cast<f32>(aimesh->mTangents[i].y);
		*(vboPtr++) = static_cast<f32>(aimesh->mTangents[i].z);
	}
	vbo.UnmapStorage();
}
void Model::LoadIndices(aiMesh* aimesh, Buffer& ebo)
{
	u32* eboPtr = static_cast<u32*>(ebo.MapStorage(GL_WRITE_ONLY));
	if (!eboPtr)
	{
		CONSOLE_ERROR("Error on mapping element buffer storage");
		return;
	}

	for (u32 i = 0; i < aimesh->mNumFaces; i++)
	{
		const aiFace& face = aimesh->mFaces[i];
		for (i32 i = 0; i < face.mNumIndices; i++)
			*(eboPtr++) = static_cast<u32>(face.mIndices[i]);
	}
	ebo.UnmapStorage();
}
Texture2D* Model::GetMaterialTexture(aiMaterial* material, aiTextureType type)
{
	aiString fileName;
	if (material->GetTexture(type, 0, &fileName) == aiReturn_SUCCESS)
		return &TextureManager::Get().GetTextureByPath(GetTexturesPath() / fileName.C_Str());

	return nullptr;
}
