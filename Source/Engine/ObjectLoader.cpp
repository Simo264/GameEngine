#include "ObjectLoader.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Components.hpp"

#include "Engine/Graphics/VertexArray.hpp"
#include "Engine/Graphics/Buffer.hpp"
#include "Engine/Graphics/Texture2D.hpp"

#include "Engine/Subsystems/TextureManager.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

ObjectLoader::ObjectLoader(const fspath& filePath)
  : mesh{},
    material{}
{
  _scene = _importer.ReadFile(filePath.string().c_str(),
    aiProcess_Triangulate | 
    aiProcess_GenSmoothNormals | 
    aiProcess_CalcTangentSpace | 
    aiProcess_FlipUVs | 
    aiProcess_JoinIdenticalVertices);

  if (!_scene || _scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_scene->mRootNode)
    CONSOLE_ERROR("ERROR::ASSIMP::{}", _importer.GetErrorString());
}

void ObjectLoader::LoadMesh(int i)
{
  mesh = _scene->mMeshes[i];
  if (mesh->mMaterialIndex >= 0)
    material = _scene->mMaterials[mesh->mMaterialIndex];
}

void ObjectLoader::LoadVertices(Buffer& vbo) const
{
  float* vboPtr = reinterpret_cast<float*>(vbo.MapStorage(GL_WRITE_ONLY));
  if (!vboPtr)
  {
    CONSOLE_WARN("Error on mapping vertex buffer storage");
    return;
  }
  
  for (uint32_t i = 0; i < mesh->mNumVertices; i++)
  {
    const aiVector3D& vertPos = mesh->mVertices[i];
    const aiVector3D& vertNor = mesh->mNormals[i];
    vec2f textureCoord{ 0.0f, 0.0f };
    if (mesh->HasTextureCoords(0))
      textureCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

    /* position */
    *(vboPtr++) = static_cast<float>(mesh->mVertices[i].x);
    *(vboPtr++) = static_cast<float>(mesh->mVertices[i].y);
    *(vboPtr++) = static_cast<float>(mesh->mVertices[i].z);
    /* texture coordinates */
    *(vboPtr++) = static_cast<float>(textureCoord.x);
    *(vboPtr++) = static_cast<float>(textureCoord.y);
    /* normal */
    *(vboPtr++) = static_cast<float>(mesh->mNormals[i].x);
    *(vboPtr++) = static_cast<float>(mesh->mNormals[i].y);
    *(vboPtr++) = static_cast<float>(mesh->mNormals[i].z);
    /* tangent */
    *(vboPtr++) = static_cast<float>(mesh->mTangents[i].x);
    *(vboPtr++) = static_cast<float>(mesh->mTangents[i].y);
    *(vboPtr++) = static_cast<float>(mesh->mTangents[i].z);
  }
  
  vbo.UnmapStorage();
}
void ObjectLoader::LoadIndices(Buffer& ebo) const
{
  uint32_t* eboPtr = reinterpret_cast<uint32_t*>(ebo.MapStorage(GL_WRITE_ONLY));
  if (!eboPtr)
  {
    CONSOLE_WARN("Error on mapping element buffer storage");
    return;
  }
  
  for (uint32_t i = 0; i < mesh->mNumFaces; i++)
  {
    const aiFace& face = mesh->mFaces[i];
    *(eboPtr++) = static_cast<uint32_t>(face.mIndices[0]);
    *(eboPtr++) = static_cast<uint32_t>(face.mIndices[1]);
    *(eboPtr++) = static_cast<uint32_t>(face.mIndices[2]);
  }

  ebo.UnmapStorage();
}
Texture2D* ObjectLoader::GetTexture(aiTextureType type) const
{
  if (material->GetTextureCount(type) <= 0)
    return nullptr;

  aiString fileName;
  if (material->GetTexture(type, 0, &fileName) != AI_SUCCESS)
    return nullptr;

  return TextureManager::Instance()->GetTextureByPath(TEXTURES_PATH / fileName.C_Str());
}
