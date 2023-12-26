#pragma once

#include "../Core.hh"
#include "../Graphics/VertexArray.hh"
#include "../Texture2D.hh"

struct MeshData
{
	uint32_t	verticesDataSize;
	float*		verticesData;
	
	uint32_t	indicesDataSize;
	uint32_t* indicesData;

	MeshData()
	{
		verticesDataSize = 0;
		indicesDataSize  = 0;

		verticesData = nullptr;
		indicesData  = nullptr;
	}
	MeshData(uint32_t vertSz, float* vertData, uint32_t indSz, uint32_t* indData)
	{
		verticesDataSize = vertSz;
		indicesDataSize  = indSz;

		verticesData = vertData;
		indicesData	 = indData;
	}
};

class Mesh
{
public:
	Mesh();
	~Mesh() = default;

	void Create(MeshData& data, VAConfiguration& config);
	void Destroy();

	VertexArray vertexArray;

	Texture2D* textureDiffuse;
	Texture2D* textureSpecular;
};

