#pragma once

#include "InstancedMesh.hh"

class InstancedCubeMesh : public InstancedMesh
{
public:
	InstancedCubeMesh(uint32_t nMaxInstances);
	~InstancedCubeMesh() = default;
};