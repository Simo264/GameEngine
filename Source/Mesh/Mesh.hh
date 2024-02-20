#pragma once

#include "../Core.hh"
#include "../VertexArray.hh"

/* -------------------------------------------------------------------------- 
	The mesh class takes vertex data, binds VAOs, VBOs, drawing orders, etc, 
	and draws it. Other classes can inherit from this class.
	-------------------------------------------------------------------------- */
class Mesh
{
public:
	Mesh() : diffuse{ nullptr }, specular{ nullptr } {}

	void InitMesh(VertexArrayData& data, VertexArrayConfig& config);
	void Draw();
	void Destroy() const;

	VertexArray vertexArray;
	class Texture2D* diffuse;
	class Texture2D* specular;
};