#include "StaticMesh.hh"
#include "../ObjectLoader.hh"
#include "../Shader.hh"

StaticMesh::StaticMesh(Path objFilePath) : RootComponent(), Mesh()
{
	ObjectLoader loader(objFilePath);
	loader.LoadStaticMesh(this);
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = this->GetModel();
	shader->SetMat4f("UModel", model);
	
	this->DrawMesh(shader);
}

void StaticMesh::DestroyStaticMesh()
{
	this->DestroyMesh();
}