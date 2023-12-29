#pragma once

#include "../Core.hh"
#include "../Texture2D.hh"
#include "../Graphics/VertexArray.hh"
#include "../Graphics/Shader.hh"


struct MeshData
{
	uint32_t	vertDataSize;
	float*		vertData;
	
	uint32_t	indDataSize;
	uint32_t* indData;

	MeshData();
	MeshData(uint32_t vertSz, float* vData, uint32_t indSz, uint32_t* iData);
};

// Base class for the every drawable object
// -----------------------------------------
class Mesh
{
public:
	Mesh() : diffuse{ nullptr }, specular{ nullptr } {}
	~Mesh() = default;

	void InitMesh(MeshData& data, VAConfiguration& config);
	void DestroyMesh();
	void Draw(Shader* shader, const mat4f& model);

	VertexArray vertexArray;
	Texture2D*	diffuse;
	Texture2D*	specular;
};

