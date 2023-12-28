#pragma once

#include "../InstancedMesh.hh"

class InstancedCube : public InstancedMesh
{
public:
	InstancedCube(uint32_t nMaxInstances);
	~InstancedCube() = default;
};