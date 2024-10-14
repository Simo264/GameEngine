#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Mesh.hpp"

/**
 * A Skeletal object refers to a three-dimensional model that uses an animation system based on a skeleton. 
 * This type of model allows the deformation of the mesh geometry by means of a set of bones (bones) 
 * that make up the skeleton, which controls the movement and animation of the model.
 * 
 * A Skeletal Model consists of two main parts:
 * 1. Mesh: The basic geometry of the object, consisting of vertices, indices, normals and other information. 
 * The mesh defines the physical appearance of the model.
 * 
 * 2. Skeleton: This is a hierarchy of bones, where each bone can affect one or more vertices in the mesh. 
 * Bones are connected to each other hierarchically (parent-child), and transformations 
 * (translations, rotations, scaling) applied to a parent bone are propagated to its children.
 */
class Skeletal
{
public:
	Skeletal(StringView skeletalPath);
	~Skeletal() = default;

  struct BoneInfo
  {
    /* ID is index in finalBoneMatrices */
    i32 id;
    /* Offset matrix transforms vertex from model space to bone space */
    mat4f offset;
  };

  auto& GetBoneInfoMap() { return _boneInfoMap; }
  i32& GetBoneCount() { return _boneCounter; }

  Vector<Mesh> meshes;
  String strPath;

private:
  void ProcessNode(struct aiNode* node, const struct aiScene* scene);
  void LoadVertices(struct aiMesh* aimesh, class Buffer& vbo);
  void LoadIndices(struct aiMesh* aimesh, class Buffer& ebo);
  Texture2D* GetMaterialTexture(struct aiMaterial* material, enum aiTextureType type);

  Map<String, BoneInfo> _boneInfoMap;
  i32 _boneCounter;

};
