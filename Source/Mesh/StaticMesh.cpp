#include "StaticMesh.hh"
#include "../ObjectLoader.hh"
#include "../Shader.hh"

StaticMesh::StaticMesh(Path objFilePath) : RootComponent(), Mesh()
{
	ObjectLoader loader(objFilePath);
	loader.LoadMesh(this);
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = this->GetModel();
	shader->SetMat4f("UModel", model);
	this->DrawMesh();
}

void StaticMesh::DestroyStaticMesh() const
{
	this->DestroyMesh();
}