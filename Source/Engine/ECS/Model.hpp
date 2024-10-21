#pragma once

#include "Core/Core.hpp"
#include "Mesh.hpp"

/**
 * @brief
 * A Model is a three-dimensional object whose geometry and position do not change over time. 
 * Consists of one or more mesh with vertices, indices and texture information 
 * (such as diffuse, specular, normal map, etc.), which are loaded once into memory and remain unchanged 
 * during the rendering cycle.
 * 
 * Unlike skeletals, which can change their form (e.g. using skeleton or keyframe animations), 
 * a model is rendered in exactly the same configuration in each frame. 
 * The applicable transformations (such as translations, rotations and scaling) are global, 
 * i.e. they can move the entire model in space, but the internal structure of the geometry does not change.
 * 
 * Models are often used to represent objects in the game world that do not require modification, 
 * such as buildings, rocks, furniture, or other scenic elements that remain fixed in their form and structure.
 */
class Model
{
public:
	Model(const fs::path& modelPath);
	~Model() = default;

	Vector<Mesh> meshes;
	String strPath;

	void DestroyModel();
	void DrawModel(i32 mode);

private:
	void ProcessNode(struct aiNode* node, const struct aiScene* scene);
};