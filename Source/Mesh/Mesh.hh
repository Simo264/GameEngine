#pragma once

#include "../Core.hh"
#include "../NonCopyable.hh"
#include "../VertexArray.hh"


/* -------------------------------------------------------------------------- 
	The mesh class takes vertex data, binds VAOs, VBOs, drawing orders, etc, 
	and draws it.
	Other classes can inherit from this class.
	-------------------------------------------------------------------------- */
class Mesh : public NonCopyable
{
public:
	Mesh() : diffuse{ nullptr }, specular{ nullptr } {}

	void InitMesh(VertexArrayData& data, VertexArrayConfig& config);
	void DestroyMesh() const;
	void DrawMesh();

	VertexArray vertexArray;
	class Texture2D* diffuse;
	class Texture2D* specular;
};