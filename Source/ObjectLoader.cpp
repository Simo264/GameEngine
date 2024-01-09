#include "ObjectLoader.hh"
#include "Texture2D.hh"
#include "Logger.hh"

#include "Mesh/StaticMesh.hh"

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
  uint32_t nMeshes = _scene->mNumMeshes;
  staticMesh->SetMeshes(nMeshes);
  for (int i = 0; i < nMeshes; i++)
  {
    aiMesh* aimesh = _scene->mMeshes[i];
    Mesh* mesh = staticMesh->GetMesh(i);
    LoadMesh(aimesh, mesh);
  }
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void ObjectLoader::LoadMesh(const aiMesh* aimesh, Mesh* mesh)
{
  Vector<float> vertices;
  Vector<uint32_t> indices;
  LoadVertices(aimesh, vertices);
  LoadIndices(aimesh, indices);

  const aiMaterial* material = _scene->mMaterials[aimesh->mMaterialIndex];
  auto diffuse = GetTexture(material, "diffuse");
  auto specular = GetTexture(material, "specular");

  // default configuration
  VertexArrayConfig config{ 3,3,2 }; //(3)position + (3)normal + (2)textCoords

  VertexArrayData data{
    vertices.size() * sizeof(float),
    vertices.data(),
    indices.size() * sizeof(uint32_t),
    indices.data()};
  
  mesh->InitMesh(data, config);
  mesh->diffuse = diffuse;
  mesh->specular = specular;
}

void ObjectLoader::LoadVertices(const aiMesh* aimesh, Vector<float>& vertDest)
{
  const uint32_t nVertices = aimesh->mNumVertices;
  const uint64_t size = nVertices * 8; // 8: 3(position) + 3(normals) + 2(textcoord)
  vertDest.reserve(size); 

  for (uint32_t i = 0; i < nVertices; i++)
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
  const uint32_t nFaces = aimesh->mNumFaces;
  const uint64_t size = nFaces * 3; // 3: number of vertices per triangle 
  indDest.reserve(size); 

  for (uint32_t i = 0; i < nFaces; i++)
  {
    const aiFace& face = aimesh->mFaces[i];
    indDest.insert(indDest.end(), { face.mIndices[0],face.mIndices[1],face.mIndices[2] });
  }
}

Texture2D* ObjectLoader::GetTexture(const aiMaterial* material, const char* textureType)
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
