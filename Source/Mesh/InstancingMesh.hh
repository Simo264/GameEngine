#pragma once

#include "../Core.hh"
#include "../NonCopyable.hh"
#include "../VertexArray.hh"

class InstancingMesh : public NonCopyable
{
public:
	/**
	 * .
	 * 
	 * \param mesh: copy mesh data
	 * \param nMaxInstances: used to initialize instance buffer
	 */
	InstancingMesh(class Mesh* mesh, uint32_t nMaxInstances);

	void AddInstance(const Mat4f& instanceModel);
	void SetInstanceModel(uint32_t instance, const Mat4f& model);

	void DrawInstMesh();
	void DestroyInstMesh() const;

	class Texture2D* diffuse;
	class Texture2D* specular;

private:
	void InitializeInstance(class Mesh* mesh);

	VertexArray _vertexArray;
	uint32_t _instanceBuffer; // vertex buffer

	uint32_t _maxNumInstances;
	uint32_t _nInstancesToDraw;
};

