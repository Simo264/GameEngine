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
    aiProcess_Triangulate | 
    aiProcess_GenSmoothNormals | 
    aiProcess_FlipUVs | 
    aiProcess_JoinIdenticalVertices);

  if (!_scene || _scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_scene->mRootNode)
  {
    CONSOLE_ERROR("ERROR::ASSIMP::{}", _importer.GetErrorString());
    throw RuntimeError("");
  }
}

void ObjectLoader::LoadStaticMesh(StaticMesh* staticMesh, uint32_t meshIndex)
{
  aiMesh* aimesh = _scene->mMeshes[meshIndex];
  LoadMeshData(aimesh, staticMesh);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void ObjectLoader::LoadMeshData(const aiMesh* aimesh, Mesh* mesh)
{
  const uint64_t vertDataSize = aimesh->mNumVertices * 8; // 8: 3(position) + 3(normals) + 2(textcoord)
  const uint64_t indDatasize = aimesh->mNumFaces * 3; // 3: number of vertices per triangle 
  
  /* Create empty mesh */
  VertexArrayData data;
  data.vertDataSize = vertDataSize * sizeof(float);
  data.vertData = nullptr;
  data.indDataSize = indDatasize * sizeof(uint32_t);
  data.indData = nullptr;
  /* Default configuration */ 
  VertexArrayConfig config;
  config.PushAttributes({ 3,3,2 }); //(3)position + (3)normal + (2)textCoords

  mesh->InitMesh(data, config);

  /* Now fill mesh vertex buffer */
  LoadVertices(aimesh, mesh->vertexArray.VertexBufferID());

  /* Now fill mesh index buffer */
  LoadIndices(aimesh, mesh->vertexArray.IndexBufferID());
  
  if (aimesh->mMaterialIndex >= 0)
  {
    const aiMaterial* material = _scene->mMaterials[aimesh->mMaterialIndex];
    mesh->diffuse = GetTexture(material, "diffuse");
    mesh->specular = GetTexture(material, "specular");
  }
}

void ObjectLoader::LoadVertices(const aiMesh* aimesh, uint32_t writeBuffer)
{
  glBindBuffer(GL_ARRAY_BUFFER, writeBuffer);
  float* vboPtr = static_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
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
  for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
  {
    const aiFace& face = aimesh->mFaces[i];
    *(eboPtr++) = (uint32_t)face.mIndices[0];
    *(eboPtr++) = (uint32_t)face.mIndices[1];
    *(eboPtr++) = (uint32_t)face.mIndices[2];
  }
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
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

  return TexturesManager::Instance().GetTexture(ROOT_PATH / fileName.C_Str());
}
