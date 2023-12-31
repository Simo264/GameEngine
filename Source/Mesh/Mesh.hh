#pragma once

#include "../Core.hh"
#include "../UncopyableObject.hh"
#include "../Graphics/VertexArray.hh"

// The mesh class takes vertex data, binds VAOs, VBOs, drawing orders, etc, and draws it.
// Other classes can inherit from this class.
class Mesh : public UncopyableObject
{
public:
	Mesh() : diffuse{ nullptr }, specular{ nullptr } {}

	void InitMesh(VertexArrayData& data, VertexArrayConfig& config);
	void DestroyMesh();
	void Draw(class Shader* shader);

	VertexArray vertexArray;
	class Texture2D* diffuse;
	class Texture2D* specular;
};