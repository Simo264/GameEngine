#include "StaticMesh.hh"
#include "../ObjectLoader.hh"
#include "../Shader.hh"

StaticMesh::StaticMesh(Path objFilePath) : RootComponent(), NonCopyable()
{
	_nMeshes = 0;

	ObjectLoader loader(objFilePath);
	loader.LoadStaticMesh(this);
}

void StaticMesh::SetMeshes(uint32_t nMeshes)
{
	_nMeshes = nMeshes;
	_meshBuffer = std::make_unique<Mesh[]>(nMeshes);
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = this->GetModel();
	shader->SetMat4f("Model", model);
	
	std::for_each_n(&_meshBuffer[0], _nMeshes, [&shader](Mesh& mesh) {
		mesh.Draw(shader);
		});
}

void StaticMesh::DestroyStaticMesh()
{
	std::for_each_n(&_meshBuffer[0], _nMeshes, [](Mesh& mesh) {
		mesh.DestroyMesh();
		});
	
	_meshBuffer.reset();
}