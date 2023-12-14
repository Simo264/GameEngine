#pragma once

#include "Mesh.hh"
#include "../Actor.hh"

class CubeMesh : public Mesh, public Actor
{
public:
	CubeMesh();
	~CubeMesh() = default;
};

