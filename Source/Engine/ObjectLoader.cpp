#include "ObjectLoader.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Graphics/VertexBuffer.hpp"
#include "Engine/Graphics/ElementBuffer.hpp"
#include "Engine/Graphics/Texture2D.hpp"

#include "Engine/Subsystems/TextureManager.hpp"

#include "Engine/Components.hpp"



/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

ObjectLoader::ObjectLoader(const fspath& filePath)
{
  _scene = _importer.ReadFile(filePath.string().c_str(),
    aiProcess_Triangulate | 
    aiProcess_GenSmoothNormals | 
    aiProcess_FlipUVs | 
    aiProcess_JoinIdenticalVertices);

  if (!_scene || _scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_scene->mRootNode)
  {
    CONSOLE_ERROR("ERROR::ASSIMP::{}", _importer.GetErrorString());
    return;
  }
}

void ObjectLoader::LoadMesh(VertexBuffer& vbo, ElementBuffer& ebo)
{
  aiMesh* aimesh = _scene->mMeshes[0];
  const uint64_t vertexDataSizeBytes  = aimesh->mNumVertices * 8 * sizeof(float);
  const uint64_t indDatasizeBytes     = aimesh->mNumFaces * 3 * sizeof(uint32_t);

  if (vertexDataSizeBytes > 0)
  {
    vbo.Create();
    vbo.CreateStorage(vertexDataSizeBytes, nullptr, GL_STATIC_DRAW);
    LoadVertices(aimesh, vbo);
  }
  if (indDatasizeBytes > 0)
  {
    ebo.Create();
    ebo.CreateStorage(indDatasizeBytes, nullptr, GL_STATIC_DRAW);
    LoadIndices(aimesh, ebo);
  }

  /* Load materials */
  if (aimesh->mMaterialIndex >= 0)
  {
    const aiMaterial* aimaterial = _scene->mMaterials[aimesh->mMaterialIndex];
    material.diffuse  = LoadTexture(aimaterial, "diffuse");
    material.specular = LoadTexture(aimaterial, "specular");
  }
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void ObjectLoader::LoadVertices(const aiMesh* aimesh, VertexBuffer& vbo)
{
  float* vboPtr = reinterpret_cast<float*>(vbo.MapStorage(GL_WRITE_ONLY));
  if (!vboPtr)
  {
    CONSOLE_WARN("Error on mapping vertex buffer storage");
    return;
  }
  
  for (uint32_t i = 0; i < aimesh->mNumVertices; i++)
  {
    const aiVector3D& vertPos = aimesh->mVertices[i];
    const aiVector3D& vertNor = aimesh->mNormals[i];
    Vec2f vertTc;
    if (aimesh->HasTextureCoords(0))
      vertTc = { aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y };
    else
      vertTc = { 0.0f, 0.0f };
    
    *vboPtr = static_cast<float>(vertPos.x); vboPtr++;
    *vboPtr = static_cast<float>(vertPos.y); vboPtr++;
    *vboPtr = static_cast<float>(vertPos.z); vboPtr++;
    *vboPtr = static_cast<float>(vertNor.x); vboPtr++;
    *vboPtr = static_cast<float>(vertNor.y); vboPtr++;
    *vboPtr = static_cast<float>(vertNor.z); vboPtr++;
    *vboPtr = static_cast<float>(vertTc.x);  vboPtr++;
    *vboPtr = static_cast<float>(vertTc.y);  vboPtr++;
  }
  
  vbo.UnmapStorage();
}

void ObjectLoader::LoadIndices(const aiMesh* aimesh, ElementBuffer& ebo)
{
  uint32_t* eboPtr = reinterpret_cast<uint32_t*>(ebo.MapStorage(GL_WRITE_ONLY));
  if (!eboPtr)
  {
    CONSOLE_WARN("Error on mapping element buffer storage");
    return;
  }
  
  for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
  {
    const aiFace& face = aimesh->mFaces[i];
    *(eboPtr++) = static_cast<uint32_t>(face.mIndices[0]);
    *(eboPtr++) = static_cast<uint32_t>(face.mIndices[1]);
    *(eboPtr++) = static_cast<uint32_t>(face.mIndices[2]);
  }

  ebo.UnmapStorage();
}

Texture2D* ObjectLoader::LoadTexture(const aiMaterial* aimaterial, const char* textureType)
{
  aiTextureType aiType = aiTextureType_NONE;
  if (std::strcmp(textureType, "diffuse") == 0)
    aiType = aiTextureType_DIFFUSE;
  else if (std::strcmp(textureType, "normal") == 0)
    aiType = aiTextureType_NORMALS;
  else if (std::strcmp(textureType, "specular") == 0)
    aiType = aiTextureType_SPECULAR;

  if (aimaterial->GetTextureCount(aiType) <= 0)
    return nullptr;

  aiString fileName;
  if (aimaterial->GetTexture(aiType, 0, &fileName) != AI_SUCCESS)
    return nullptr;

  return TextureManager::Instance().GetTextureByPath(ROOT_PATH / fileName.C_Str());
}
