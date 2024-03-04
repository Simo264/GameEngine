#include "ObjectLoader.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Graphics/VertexArray.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "Engine/ECS/StaticMeshComponent.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

ObjectLoader::ObjectLoader(const Path& filePath)
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

void ObjectLoader::LoadMesh(StaticMeshComponent* mesh)
{
  aiMesh* aimesh = _scene->mMeshes[0];
  const uint64_t vertexDataSize = aimesh->mNumVertices * 8; /* 8: 3(position), 3(normals), 2(uv/text coords) */
  const uint64_t indDatasize = aimesh->mNumFaces * 3;       /* 3: number of vertices per triangle */

  /* Store the sizes for the vertex array buffers */
  VertexBufferData data;
  data.vertexDataSize = vertexDataSize * sizeof(float);
  data.vertextDataPtr = nullptr;
  data.indexDataSize = indDatasize * sizeof(uint32_t);
  data.indexDataPtr = nullptr;
  
  /* By default layout is 3(position), 3(normals), 2(uv/text coords) */
  VertexBufferLayout layout;
  layout.PushAttributes({ 3,3,2 }); 

  /* Initialize empty buffers with calculated size */
  SharedPointer<VertexArray>& vao = mesh->vertexArray;
  vao->InitializeBuffers(layout, data, GL_STATIC_DRAW);

  /* Now fill vertex buffer with data */
  LoadVertices(aimesh, vao->VertexBufferID());

  /* Now fill index buffer with data */
  LoadIndices(aimesh, vao->IndexBufferID());

  /* Load materials */
  if (aimesh->mMaterialIndex >= 0)
  {
    const aiMaterial* aimaterial = _scene->mMaterials[aimesh->mMaterialIndex];
    material.diffuse = LoadTexture(aimaterial, "diffuse");
    material.specular = LoadTexture(aimaterial, "specular");
  }
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void ObjectLoader::LoadVertices(const aiMesh* aimesh, uint32_t writeBuffer)
{
  glBindBuffer(GL_ARRAY_BUFFER, writeBuffer);
  float* vboPtr = static_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
  if (!vboPtr)
  {
    CONSOLE_WARN("VertexBuffer pointer is null");
    return;
  }
  
  for (uint32_t i = 0; i < aimesh->mNumVertices; i++)
  {
    aiVector3D& vertPos = aimesh->mVertices[i];
    aiVector3D& vertNor = aimesh->mNormals[i];
    Vec2f vertTc;
    if (aimesh->HasTextureCoords(0))
      vertTc = { aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y };
    else
      vertTc = { 0.0f, 0.0f };
    
    *vboPtr = (float)vertPos.x; vboPtr++;
    *vboPtr = (float)vertPos.y; vboPtr++;
    *vboPtr = (float)vertPos.z; vboPtr++;
    *vboPtr = (float)vertNor.x; vboPtr++;
    *vboPtr = (float)vertNor.y; vboPtr++;
    *vboPtr = (float)vertNor.z; vboPtr++;
    *vboPtr = (float)vertTc.x;  vboPtr++;
    *vboPtr = (float)vertTc.y;  vboPtr++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
}

void ObjectLoader::LoadIndices(const aiMesh* aimesh, uint32_t writeBuffer)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, writeBuffer);
  uint32_t* eboPtr = static_cast<uint32_t*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
  if (!eboPtr)
  {
    CONSOLE_WARN("ElementBuffer pointer nullptr");
    return;
  }
  
  for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
  {
    const aiFace& face = aimesh->mFaces[i];
    *(eboPtr++) = (uint32_t)face.mIndices[0];
    *(eboPtr++) = (uint32_t)face.mIndices[1];
    *(eboPtr++) = (uint32_t)face.mIndices[2];
  }
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
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
