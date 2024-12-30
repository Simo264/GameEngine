#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Mesh.hpp"

class Texture2D;
class Buffer;
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class StaticMesh
{
public:
	StaticMesh(const fs::path& modelPath);
	~StaticMesh() = default;

	void Draw(i32 mode);
	u32 TotalVertices() const;
	u32 TotalIndices() const;

	Vector<Mesh> meshes;
	fs::path path;

private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	Buffer LoadVertices(aiMesh* aimesh);
	Buffer LoadIndices(aiMesh* aimesh);
	Texture2D* GetMaterialTexture(aiMaterial* material, u32 textureType);
};