#pragma once

#include "StaticMesh.hh"

class InstancedMesh
{
public:
	InstancedMesh(uint32_t nMaxInstances)
		: MAX_NUM_INSTANCES{ nMaxInstances }, _nInstancesToDraw{ 1 }, _instanceBuffer{ 0 } {}

	~InstancedMesh() = default;

	void Init(VAData& data, VAConfig& config);
	void Destroy();

	void		 SetInstanceNumber(uint32_t n);
	uint32_t GetInstanceNumber() const { return _nInstancesToDraw; }

	void SetInstanceModel(mat4f* model, uint32_t instance);
	void SetInstancesModelRange(uint32_t start, uint32_t n, mat4f* models);

	VertexArray vertexArray;
	
	const uint32_t MAX_NUM_INSTANCES;

private:
	void Instancing(VAConfig& config);

	uint32_t _instanceBuffer;
	uint32_t _nInstancesToDraw;
};

