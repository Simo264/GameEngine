#include "StaticMesh.hh"
#include "../ObjectLoader.hh"
#include "../Shader.hh"

StaticMesh::StaticMesh(Path objFilePath) : Mesh()
{
	this->tagName = "Static mesh";

	ObjectLoader loader(objFilePath);
	loader.LoadMesh(this);
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = transform.GetTransformation();
	shader->SetMat4f("UModel", model);
	Mesh::Draw();
}
