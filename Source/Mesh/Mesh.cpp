#include "Mesh.hh"

void Mesh::Init(uint32_t vertSize, float* vertices, uint32_t indSize, uint32_t* indices, Graphics::VAConfiguration& config)
{
	vertexArray.Init(vertSize, vertices, indSize, indices, config);
}

void Mesh::Init(uint32_t vertSize, float* vertices, Graphics::VAConfiguration& config)
{
	vertexArray.Init(vertSize, vertices, config);
}

void Mesh::Destroy()
{
	vertexArray.Destroy();
}
