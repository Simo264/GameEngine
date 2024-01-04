#include "Model.hh"
#include "../Graphics/Renderer.hh"
#include "../ObjectLoader.hh"
#include "../Logger.hh"


Model::Model(Path filePath)
{
	_nMeshes = 0;

	ObjectLoader loader(filePath);
	loader.LoadModel(this);
}

void Model::Draw(Shader* shader)
{
	for (int i = 0; i < _nMeshes; i++)
	{
		StaticMesh* mesh = &meshBuffer[i];
		mesh->Draw(shader);
	}
}

void Model::Destroy()
{
	for (int i = 0; i < _nMeshes; i++)
	{
		StaticMesh* mesh = &meshBuffer[i];
		mesh->DestroyMesh();
	}
	meshBuffer.reset();
}

void Model::Initialize(uint32_t nMeshes)
{
	meshBuffer = std::make_unique<StaticMesh[]>(nMeshes);
	_nMeshes = nMeshes;
}