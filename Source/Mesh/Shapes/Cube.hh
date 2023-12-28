#pragma once

#include "../Mesh.hh"
#include "../../Actor.hh"

class Cube : public Mesh, public Actor
{
public:
	Cube();
	~Cube() = default;
};

