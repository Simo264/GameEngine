#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/ECS/Mesh.hpp"
#include "Engine/ECS/Skeleton/Bone.hpp"

class Texture2D;
struct Vertex_P_N_UV_T_B;
struct aiMesh;
struct aiNode;
struct aiScene;
struct aiMaterial;

class SkeletonMesh
{
public:
  SkeletonMesh(const fs::path& skeletalPath);
  ~SkeletonMesh() = default;

  void DrawSkeleton(i32 mode);
  
  std::pair<Bone*, u32> FindBone(StringView boneName);

  String strPath;
  
  Vector<Mesh> meshes;
  BoneNode rootNode;
  Vector<Bone> bones;
  UnorderedMap<String, u32> boneMap; /* BoneName - boneIndex */

private:
  void ProcessNode(aiNode* node, const aiScene* scene);
  void LoadVertices(Vector<Vertex_P_N_UV_T_B>& vertices, aiMesh* aimesh);
  void LoadIndices(Vector<u32>& indices, aiMesh* aimesh);
  Texture2D* GetMaterialTexture(aiMaterial* material, u32 textureType);
  void LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh);

  void LoadBoneHierarchy(BoneNode& dest, const aiNode* src);
};
