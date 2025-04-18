#pragma once

#include "Core/Core.hpp"

class StaticMeshLoader
{
public:
	StaticMeshLoader() = default;
	~StaticMeshLoader() = default;

	void LoadDataFromFile(const fs::path& absolutePathToFile, class StaticMesh& out);

private:
	void ProcessAINode(struct aiNode* node, const struct aiScene* scene, class StaticMesh& out);
	class Buffer LoadVertices(struct aiMesh* aimesh);
	class Buffer LoadIndices(struct aiMesh* aimesh);
	void LoadMeshMaterial(const struct aiScene* scene, struct aiMesh* aimesh, class Mesh& mesh);
};