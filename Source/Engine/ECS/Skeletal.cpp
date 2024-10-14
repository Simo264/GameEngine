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

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Skeletal::Skeletal(StringView skeletalPath)
	: strPath{ skeletalPath.data() },
		meshes{},
		_boneCounter{ 0 }
{
	CONSOLE_TRACE("Loading skeletal {}...", strPath);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(skeletalPath.data(),
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
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void Skeletal::ProcessNode(aiNode* node, const aiScene* scene)
{

}
void Skeletal::LoadVertices(aiMesh* aimesh, Buffer& vbo)
{

}
void Skeletal::LoadIndices(aiMesh* aimesh, Buffer& ebo)
{

}
Texture2D* Skeletal::GetMaterialTexture(aiMaterial* material, aiTextureType type)
{
	return nullptr;
}
