#pragma once

#include "Core/Core.hpp"
#include "Mesh.hpp"

/**
 * A model contains multiple meshes, possibly with multiple textures.
 * A house, that contains a wooden balcony, a tower, and perhaps a swimming pool, could still be loaded as a single model.
 * We'll load the model via Assimp and translate it to multiple Mesh objects.
 */
class Model
{
public:
	Model(StringView modelPath);
	~Model() = default;

	Vector<Mesh> meshes;

	void DestroyModel();
	void DrawModel(i32 mode);

private:
	void ProcessNode(struct aiNode* node, const struct aiScene* scene);
	void LoadVertices(struct aiMesh* aimesh, class Buffer& vbo);
	void LoadIndices(struct aiMesh* aimesh, class Buffer& ebo);
	Texture2D* GetTexture(struct aiMaterial* material, enum aiTextureType type);
};