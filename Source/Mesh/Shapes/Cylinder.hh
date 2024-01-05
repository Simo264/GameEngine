#pragma once

#include "../StaticMesh.hh"

class Cylinder : public StaticMesh
{
public:
	Cylinder() : StaticMesh("Shapes/Cylinder/Cylinder.obj") {}
};