#pragma once

#include "Mesh.hh"

class InstancedMesh
{
public:
	InstancedMesh(uint32_t nMaxInstances);
	~InstancedMesh() = default;

	void Init(MeshData& data, VAConfiguration& config);
	void Destroy();

	void		 SetInstanceNumber(uint32_t n);
	uint32_t GetInstanceNumber() const { return _nInstancesToDraw; }

	void SetInstanceModel(mat4f* model, uint32_t instance);
	void SetInstancesModelRange(uint32_t start, uint32_t n, mat4f* models);

	VertexArray vertexArray;
	
	const uint32_t MAX_NUM_INSTANCES;

private:
	void Instancing(VAConfiguration& config);

	uint32_t _instanceBuffer;
	uint32_t _nInstancesToDraw;
};

