#include "ObjectLoader.hh"
#include "TexturesManager.hh"
#include "../Logger.hh"

std::filesystem::path ObjectLoader::assetsDirPath = std::filesystem::current_path().parent_path() / "Assets";

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void ObjectLoader::LoadSingleMeshData(
  std::filesystem::path filePath,
  vector<float>&        vertDest,
  vector<uint32_t>&     indDest,
  array<std::filesystem::path, 2>& texturePaths)
{
	filePath = assetsDirPath / filePath.lexically_normal();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.string().c_str(),
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

  if (!CheckScene(scene))
  {
    CONSOLE_ERROR("ERROR::ASSIMP::{}", importer.GetErrorString());
    return;
  }

	aiMesh* aimesh = scene->mMeshes[0];
	vertDest.reserve(aimesh->mNumVertices * 8); // 8 -> position(3)+normals(3)+textcoord(2)
	indDest.reserve(aimesh->mNumFaces * 3);     // 3 -> 3 vertices per triangle 

  LoadVertices(aimesh, vertDest);
  LoadIndices(aimesh, indDest);

  // Load textures
  const aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
  texturePaths[0] = GetTexturePath(material, "diffuse");
  texturePaths[1] = GetTexturePath(material, "specular");
  //textures[2] = LoadTexture(material, "normal");
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

bool ObjectLoader::CheckScene(const aiScene* scene)
{
  return !(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode);
}

void ObjectLoader::LoadVertices(const aiMesh* aimesh, vector<float>& vertDest)
{
  for (uint32_t i = 0; i < aimesh->mNumVertices; i++)
  {
    aiVector3D& vertPos = aimesh->mVertices[i];
    aiVector3D& vertNor = aimesh->mNormals[i];
    aiVector3D& vertTc = aimesh->mTextureCoords[0][i];

    vertDest.insert(vertDest.end(), { vertPos.x,vertPos.y,vertPos.z });
    vertDest.insert(vertDest.end(), { vertNor.x,vertNor.y,vertNor.z });
    vertDest.insert(vertDest.end(), { vertTc.x,vertTc.y });
  }
}

void ObjectLoader::LoadIndices(const aiMesh* aimesh, vector<uint32_t>& indDest)
{
  for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
  {
    const aiFace& face = aimesh->mFaces[i];
    indDest.insert(indDest.end(), { face.mIndices[0],face.mIndices[1],face.mIndices[2] });
  }
}

std::filesystem::path ObjectLoader::GetTexturePath(const struct aiMaterial* material, const char* textureType)
{
  aiTextureType aiType = aiTextureType_NONE;
  if (std::strcmp(textureType, "diffuse") == 0)
    aiType = aiTextureType_DIFFUSE;
  else if (std::strcmp(textureType, "normal") == 0)
    aiType = aiTextureType_NORMALS;
  else if (std::strcmp(textureType, "specular") == 0)
    aiType = aiTextureType_SPECULAR;

  if (material->GetTextureCount(aiType) <= 0)
    return std::filesystem::path{};

  aiString fileName;
  if (material->GetTexture(aiType, 0, &fileName) != AI_SUCCESS)
    return std::filesystem::path{};

  return std::filesystem::path{ fileName .C_Str() };
}
