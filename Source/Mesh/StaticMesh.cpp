#include "StaticMesh.hh"
#include "../ObjectLoader.hh"
#include "../Graphics/Shader.hh"

StaticMesh::StaticMesh(Path objFilePath) : RootComponent(), UncopyableObject()
{
	_meshBuffer = UniquePointer<Mesh[]>(nullptr);
	_nMeshes = 0;

	ObjectLoader loader(objFilePath);
	loader.LoadStaticMesh(this);
}

void StaticMesh::SetMeshes(uint32_t nMeshes)
{
	_nMeshes = nMeshes;
	_meshBuffer.reset(new Mesh[nMeshes]);
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = this->GetModel();
	shader->SetMat4f("Model", model);
	
	for (int i = 0; i < _nMeshes; i++)
	{
		Mesh* mesh = &_meshBuffer[i];
		mesh->Draw(shader);
	}
}

void StaticMesh::DestroyStaticMesh()
{
	for (int i = 0; i < _nMeshes; i++)
	{
		Mesh* mesh = &_meshBuffer[i];
		mesh->DestroyMesh();
	}
	_meshBuffer.reset();
}