#pragma once

#include "../Core.hh"
#include "../Graphics/VertexArray.hh"

class Mesh
{
public:
	Mesh()  = default;
	~Mesh() = default;

	void Init(uint32_t vertSize, float* vertices, uint32_t indSize, uint32_t* indices, Graphics::VAConfiguration& config);
	void Init(uint32_t vertSize, float* vertices, Graphics::VAConfiguration& config);

	void Destroy();

	Graphics::VertexArray vertexArray;
};

