#include "StaticMesh.hpp"

#include "Engine/ObjectLoader.hpp"
#include "Engine/Shader.hpp"
#include "Engine/Texture2D.hpp"

StaticMesh::StaticMesh(Path objFilePath) : Mesh()
{
  _model = objFilePath;
	tagName = "Static mesh";

	ObjectLoader loader(objFilePath);
	loader.LoadMesh(this);
}

void StaticMesh::Draw(Shader* shader)
{
	auto model = transform.GetTransformation();
	shader->SetMat4f("UModel", model);
	Mesh::Draw();
}

String StaticMesh::ToString() const
{
  OStringStream oss;
  oss << "type=StaticMesh" << "\n";
  oss << "tag=" << tagName << "\n";
  oss << "modelpath=" << _model.string() << "\n";
  oss << "texture-diffuse=" << diffuse->texturePath.string() << "\n";
  oss << "position=" << transform.position.x << "," << transform.position.y << "," << transform.position.z << "\n";
  oss << "scale=" << transform.scale.x << "," << transform.scale.y << "," << transform.scale.z << "\n";
  oss << "degrees=" << transform.degrees.x << "," << transform.degrees.y << "," << transform.degrees.z << "\n";
  return oss.str();
}
