#include "StaticMesh.hh"
#include "../ObjectLoader.hh"
#include "../Graphics/Shader.hh"

StaticMesh::StaticMesh(Path objFilePath) : RootComponent(), Mesh()
{
	ObjectLoader loader(objFilePath);
	loader.LoadStaticMesh(this);
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = this->GetModel();
	shader->SetMat4f("Model", model);
	
	Mesh::Draw(shader);
}