#pragma once

#include "../Core.hh"
#include "../Graphics/VertexArray.hh"

class InstancedMesh
{
public:
	InstancedMesh() = default;
	~InstancedMesh() = default;

	void Init(uint32_t vertSize, float* vertices, uint32_t indSize, uint32_t* indices, Graphics::VAConfiguration& config);
	void Init(uint32_t vertSize, float* vertices, Graphics::VAConfiguration& config);
	void Destroy();

	Graphics::VertexArray vertexArray;
	uint32_t instanceBuffer;

	inline static const uint32_t MAX_NUM_INSTANCES = 10;
};

