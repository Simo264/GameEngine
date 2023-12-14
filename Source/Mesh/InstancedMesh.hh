#pragma once

#include "../Core.hh"
#include "../Graphics/VertexArray.hh"

class InstancedMesh
{
public:
	InstancedMesh();
	~InstancedMesh() = default;

	void Init(uint32_t vertSize, float* vertices, uint32_t indSize, uint32_t* indices, Graphics::VAConfiguration& config);
	void Init(uint32_t vertSize, float* vertices, Graphics::VAConfiguration& config);
	void Destroy();

	void SetInstances(uint32_t n);
	uint32_t GetInstances() const { return _nInstancesToDraw; }

	void SetModel(mat4f* model, uint32_t instance);
	void SetModelsRange(uint32_t start, uint32_t end, mat4f* models);

	Graphics::VertexArray vertexArray;
	
	const uint32_t MAX_NUM_INSTANCES;

private:
	void Instancing(Graphics::VAConfiguration& config);

	uint32_t _instanceBuffer;
	uint32_t _nInstancesToDraw;

	vector<mat4f> _models;
};

