#include "Cube.hh"

#include "../../Subsystems/ObjectLoader.hh"
#include "../../Subsystems/TexturesManager.hh"

Cube::Cube() : Actor()
{
  vector<float>     vertices;
  vector<uint32_t>  indices;
  array<std::filesystem::path, 2> texturePaths; // 2 -> [diffusePath, specularPath]
  ObjectLoader::LoadSingleMeshData("Mesh/Cube/Cube.obj", vertices, indices, texturePaths);

  this->diffuse  = (!texturePaths[0].empty()) ? TexturesManager::GetTexture(texturePaths[0]) : nullptr;
  this->specular = (!texturePaths[1].empty()) ? TexturesManager::GetTexture(texturePaths[1]) : nullptr;
  
  MeshData data{ 
    (uint32_t)(vertices.size() * sizeof(float)),
    vertices.data(), 
    (uint32_t)(indices.size() * sizeof(uint32_t)),
    indices.data()};

  // default configuration
  // ---------------------------------
  VAConfiguration config;
  config.PushAttribute(3); // vec3 position    
  config.PushAttribute(3); // vec3 normals 
  config.PushAttribute(2); // vec2 text coords

  this->InitMesh(data, config);
}
