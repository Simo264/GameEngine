#include "StaticMesh.hh"
#include "../Graphics/Shader.hh"

void StaticMesh::Draw(Shader* shader)
{
	auto model = this->GetModel();
	Mesh::Draw(shader, model);
}