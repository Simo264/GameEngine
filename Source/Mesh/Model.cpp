#include "Model.hh"

#include "../Graphics/Shader.hh"
#include "../Graphics/Renderer.hh"
#include "../ObjectLoader.hh"
#include "../Logger.hh"


Model::Model(Path filePath)
{
	_nMeshes = 0;

	ObjectLoader loader(filePath);
	loader.LoadModel(this);
}

void Model::Draw(Shader* shader) const
{
	for (int i = 0; i < _nMeshes; i++)
	{
		Mesh* mesh = &meshBuffer[i];
		mesh->Draw(shader);
	}
}

void Model::DestroyModel()
{
	for (int i = 0; i < _nMeshes; i++)
	{
		Mesh* mesh = &meshBuffer[i];
		mesh->DestroyMesh();
	}
	meshBuffer.reset();
}

void Model::InitModel(uint32_t nMeshes)
{
	meshBuffer = std::make_unique<Mesh[]>(nMeshes);
	_nMeshes = nMeshes;
}