#include "ObjectLoader.hh"
#include "Logger.hh"

#include "Graphics/VertexArray.hh"

#include "Subsystems/TexturesManager.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

ObjectLoader::ObjectLoader(Path filePath)
{
  filePath = _assetsDirPath / filePath.lexically_normal();
  
  _scene = _importer.ReadFile(filePath.string().c_str(),
    aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

  if (!_scene || _scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_scene->mRootNode)
  {
    CONSOLE_ERROR("ERROR::ASSIMP::{}", _importer.GetErrorString());
    throw RuntimeError("");
  }
}

void ObjectLoader::LoadStaticMesh(StaticMesh* staticMesh)
{
  aiMesh* aimesh = _scene->mMeshes[0];
  LoadMesh(aimesh, staticMesh);
}

void ObjectLoader::LoadModel(Model* model)
{
  int nMeshes = _scene->mNumMeshes;

  model->Initialize(nMeshes);

  for (int i = 0; i < nMeshes; i++)
  {
    aiMesh* aimesh = _scene->mMeshes[i];
    LoadMesh(aimesh, &model->meshBuffer[i]);
  }
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void ObjectLoader::LoadMesh(const aiMesh* aimesh, StaticMesh* staticMesh)
{
  const int nVertices = aimesh->mNumVertices;
  const int nFaces = aimesh->mNumFaces;

  Vector<float> vertices;
  Vector<uint32_t> indices;
  vertices.reserve(nVertices * 8); // 8 -> position(3)+normals(3)+textcoord(2)
  indices.reserve(nFaces * 3);     // 3 -> 3 vertices per triangle 
  LoadVertices(aimesh, vertices);
  LoadIndices(aimesh, indices);

  const aiMaterial* material = _scene->mMaterials[aimesh->mMaterialIndex];
  auto diffuse = GetTexture(material, "diffuse");
  auto specular = GetTexture(material, "specular");

  // default configuration
  // ---------------------------------
  VAConfig config;
  config.PushAttribute(3); // vec3 position    
  config.PushAttribute(3); // vec3 normals 
  config.PushAttribute(2); // vec2 text coords

  VAData data;
  data.vertData = vertices.data();
  data.vertDataSize = vertices.size() * sizeof(float);
  data.indData = indices.data();
  data.indDataSize = indices.size() * sizeof(uint32_t);

  staticMesh->InitMesh(data, config);
  staticMesh->diffuse = diffuse;
  staticMesh->specular = specular;
}

void ObjectLoader::LoadVertices(const aiMesh* aimesh, Vector<float>& vertDest)
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

void ObjectLoader::LoadIndices(const aiMesh* aimesh, Vector<uint32_t>& indDest)
{
  for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
  {
    const aiFace& face = aimesh->mFaces[i];
    indDest.insert(indDest.end(), { face.mIndices[0],face.mIndices[1],face.mIndices[2] });
  }
}

Texture2D* ObjectLoader::GetTexture(const struct aiMaterial* material, const char* textureType)
{
  aiTextureType aiType = aiTextureType_NONE;
  if (std::strcmp(textureType, "diffuse") == 0)
    aiType = aiTextureType_DIFFUSE;
  else if (std::strcmp(textureType, "normal") == 0)
    aiType = aiTextureType_NORMALS;
  else if (std::strcmp(textureType, "specular") == 0)
    aiType = aiTextureType_SPECULAR;

  if (material->GetTextureCount(aiType) <= 0)
    return nullptr;

  aiString fileName;
  if (material->GetTexture(aiType, 0, &fileName) != AI_SUCCESS)
    return nullptr;

  return TexturesManager::GetTexture(fileName.C_Str());
}
