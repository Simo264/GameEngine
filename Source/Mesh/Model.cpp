#include "Model.hh"

#include "../Graphics/Shader.hh"
#include "../Graphics/Renderer.hh"
#include "../ObjectLoader.hh"
#include "../Logger.hh"


Model::Model(Path objFilePath)
{
	_nMeshes = 0;

	ObjectLoader loader(objFilePath);
	loader.LoadModel(this);
}

void Model::SetMeshNumber(uint32_t nMeshes)
{
	meshBuffer = std::make_unique<Mesh[]>(nMeshes);
	_nMeshes = nMeshes;
}

void Model::Draw(Shader* shader) const
{
	auto model = this->GetModel();
	shader->SetMat4f("Model", model);

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

