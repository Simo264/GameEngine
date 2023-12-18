#include "Mesh.hh"

void Mesh::Init(MeshData& data, Graphics::VAConfiguration& config)
{
	textureDiffuse	= nullptr;
	textureSpecular = nullptr;
	vertexArray.Init(data.verticesDataSize, data.verticesData, data.indicesDataSize, data.indicesData, config);
}

void Mesh::Destroy()
{
	vertexArray.Destroy();
}
