#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/ECS/Mesh.hpp"
#include "Bone.hpp"

struct Vertex_P_N_UV_T_B;
struct aiMesh;
struct aiNode;
struct aiScene;


/**
 * @brief
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
class Skeleton
{
public:
	Skeleton(const fs::path& skeletalPath);
	~Skeleton() = default;

  Vector<Mesh> meshes;
  String strPath;

  auto& GetBoneInfoMap() { return _boneInfoMap; }
  i32& GetBoneCount() { return _boneCounter; }
  void DrawSkeleton(i32 mode);

private:
  void ProcessNode(aiNode* node, const aiScene* scene);
  void LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh);

  Map<String, BoneInfo> _boneInfoMap;
  i32 _boneCounter;
};
