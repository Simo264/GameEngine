#include "StaticMesh.hpp"

#include "Engine/ObjectLoader.hpp"
#include "Engine/Shader.hpp"
#include "Engine/Texture2D.hpp"

StaticMesh::StaticMesh(Path objFilePath) : Mesh()
{
  modelPath = objFilePath;
	tagName = "Static mesh";

	ObjectLoader loader(modelPath);
	loader.LoadMesh(this);
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = transform.GetTransformation();
	shader->SetMat4f("UModel", model);
	Mesh::Draw();
}
