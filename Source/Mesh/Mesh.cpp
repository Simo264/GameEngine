#include "Mesh.hh"

Mesh::Mesh()
{
	textureDiffuse = nullptr;
	textureSpecular = nullptr;
}

void Mesh::Create(MeshData& data, VAConfiguration& config)
{
	vertexArray.Create(data.verticesDataSize, data.verticesData, data.indicesDataSize, data.indicesData, config);
}

void Mesh::Destroy()
{
	vertexArray.Destroy();
}
