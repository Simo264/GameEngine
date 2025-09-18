#pragma once

#include "Core/Core.hpp"
#include "Engine/Components/StaticMesh.hpp"

class Mesh;
class Buffer;
struct aiNode;
struct aiMesh;
struct aiScene;

class StaticMeshLoader
{
public:
	StaticMeshLoader() = default;

	void LoadDataFromFile(const fs::path& absolutePathToFile, Components::StaticMesh& out);

private:
	void __ProcessAINode(aiNode* node, const aiScene* scene, Components::StaticMesh& out);
	Buffer __LoadVertices(aiMesh* aimesh);
	Buffer __LoadIndices(aiMesh* aimesh);
	void __LoadMeshMaterial(const aiScene* scene, aiMesh* aimesh, Mesh& mesh);
};