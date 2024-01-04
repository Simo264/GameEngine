#include "Plane.hh"

#include "../../ObjectLoader.hh"
#include "../../Subsystems/TexturesManager.hh"

Plane::Plane() : StaticMesh()
{
  Vector<float>     vertices;
  Vector<uint32_t>  indices;
  Array<Path, 2>    texturePaths; // 2 -> [diffusePath, specularPath]
  
  ObjectLoader loader("Shapes/Plane/Plane.obj");
  loader.LoadMesh(vertices, indices, texturePaths);

  if (!texturePaths[0].empty())
    this->diffuse = TexturesManager::GetTexture(texturePaths[0]);
  if (!texturePaths[1].empty())
    this->specular = TexturesManager::GetTexture(texturePaths[1]);

  VAData data{
    (uint32_t)(vertices.size() * sizeof(float)),
    vertices.data(),
    (uint32_t)(indices.size() * sizeof(uint32_t)),
    indices.data() };

  // default configuration
  // ---------------------------------
  VAConfig config;
  config.PushAttribute(3); // vec3 position    
  config.PushAttribute(3); // vec3 normals 
  config.PushAttribute(2); // vec2 text coords

  this->InitMesh(data, config);
}