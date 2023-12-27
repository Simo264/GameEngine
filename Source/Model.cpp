#include "Model.hh"
#include "Logger.hh"
#include "Graphics/Renderer.hh"
#include "ResourceManager/TexturesManager.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Model::Model(std::filesystem::path filePath)
{
  std::filesystem::path assetsDirPath = std::filesystem::current_path().parent_path() / "Assets";
  filePath = assetsDirPath / filePath.lexically_normal();
	LoadModel(filePath);
}

void Model::Draw(Shader* shader)
{
  mat4f model = Actor::GetModel();
  shader->SetMat4f("Model", model);
  for (uint32_t i = 0; i < _numMeshes; i++)
  {
    auto& mesh = _meshPool[i];
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (mesh.textureDiffuse)
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mesh.textureDiffuse->textureID);
    }
    if (mesh.textureSpecular)
    {
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, mesh.textureSpecular->textureID);
    }

    Renderer::DrawIndexed(mesh.vertexArray);
  }
}

void Model::Destroy()
{
  for (uint32_t i = 0; i < _numMeshes; i++)
  {
    auto& mesh = _meshPool[i];
    mesh.Destroy();
  }

  _meshPool.reset();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Model::LoadModel(std::filesystem::path filePath)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filePath.string().c_str(),
    aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    CONSOLE_ERROR("ERROR::ASSIMP::{}", importer.GetErrorString());
    return;
  }

  // Allocates memory on the heap to hold `_numMeshes` mesh objects
  _numMeshes = scene->mNumMeshes;
  _meshPool  = std::make_unique<Mesh[]>(_numMeshes);

  CONSOLE_TRACE("Loading model {}: num meshes {}", filePath.string(), _numMeshes);
  for (uint32_t i = 0; i < _numMeshes; i++)
  {
    aiMesh* aimesh = scene->mMeshes[i];
    LoadMesh(i, scene, aimesh);
  }
}

void Model::LoadMesh(uint32_t index, const struct aiScene* scene, const struct aiMesh* aimesh)
{
  vector<float>     vertices;
  vector<uint32_t>  indices;
  vertices.reserve(aimesh->mNumVertices * 8); // 8 -> position(3)+normals(3)+textcoord(2)
  indices.reserve(aimesh->mNumFaces * 3);     // 3 -> 3 vertices per triangle 

  // load vertices
  for (uint32_t i = 0; i < aimesh->mNumVertices; i++)
  {
    aiVector3D& vertPos = aimesh->mVertices[i];
    aiVector3D& vertNor = aimesh->mNormals[i];
    aiVector3D& vertTc  = aimesh->mTextureCoords[0][i];

    vertices.insert(vertices.end(), { vertPos.x,vertPos.y,vertPos.z });
    vertices.insert(vertices.end(), { vertNor.x,vertNor.y,vertNor.z });
    vertices.insert(vertices.end(), { vertTc.x,vertTc.y });
  }

  // load indices
  for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
  {
    const aiFace& face = aimesh->mFaces[i];
    indices.insert(indices.end(), { face.mIndices[0],face.mIndices[1],face.mIndices[2] });
  }

  VAConfiguration config;
  config.PushAttribute(3); //layout=0 vec3 aPos
  config.PushAttribute(3); //layout=1 vec3 aNormals
  config.PushAttribute(2); //layout=2 vec2 aTexCoords

  MeshData data{ 
    (uint32_t)vertices.size() * 4, 
    vertices.data(), 
    (uint32_t)indices.size() * 4, 
    indices.data()
  };

  Mesh& mesh = _meshPool[index];
  mesh.Create(data, config);

  // load textures
  const aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
  Texture2D* diffuse  = LoadTexture(material, "diffuse");
  Texture2D* specular = LoadTexture(material, "specular");
  //Texture2D* normal   = LoadTexture(material, "normal");
  mesh.textureDiffuse  = diffuse;
  mesh.textureSpecular = specular;
  //mesh.normal   = normal;
}

Texture2D* Model::LoadTexture(const struct aiMaterial* material, const char* textureType)
{
  aiTextureType aiType = aiTextureType_NONE;
  if (strcmp(textureType, "diffuse") == 0)
    aiType = aiTextureType_DIFFUSE;
  else if (strcmp(textureType, "normal") == 0)
    aiType = aiTextureType_NORMALS;
  else if (strcmp(textureType, "specular") == 0)
    aiType = aiTextureType_SPECULAR;

  if (material->GetTextureCount(aiType) <= 0)
    return nullptr;

  aiString fileName;
  if (material->GetTexture(aiType, 0, &fileName) != AI_SUCCESS)
    return nullptr;

  // Load textures from current model directory
  Texture2D* texture = TexturesManager::GetTexture(fileName.C_Str());
  if (!texture)
    CONSOLE_WARN("Model::LoadTexture: {} does not exists", fileName.C_Str());
  
  return texture;
}
