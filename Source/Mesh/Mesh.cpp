#include "Mesh.hh"

void Mesh::Init(MeshData& data, Graphics::VAConfiguration& config)
{
	vertexArray.Init(data.verticesDataSize, data.verticesData, data.indicesDataSize, data.indicesData, config);
}

void Mesh::Destroy()
{
	vertexArray.Destroy();
}
