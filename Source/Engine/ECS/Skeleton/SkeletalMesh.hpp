#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Engine/Graphics/Mesh.hpp"
#include "Engine/ECS/Skeleton/Bone.hpp"
#include "Engine/ECS/Animation/Animator.hpp"

class Texture2D;
struct Vertex_P_N_UV_T_B;
struct aiMesh;
struct aiNode;
struct aiScene;
struct aiMaterial;

/**
 * @class SkeletonMesh
 * @brief Represents a skeletal mesh for rendering and manipulation of skeleton-based models.\
 */
class SkeletalMesh
{
public:
  /** @brief Default constructor for the SkeletonMesh class. */
  SkeletalMesh() :
    nrMeshes{ 0 },
    meshes{ nullptr },
    nrBones{ 0 },
    bones{ nullptr },
    boneNames{ nullptr },
    id{ 0 },
    rootNode{}
  {}
  /** @brief Default destructor for the SkeletonMesh class. */
  ~SkeletalMesh() = default;

  static constexpr u32 GetMaxNumBones()
  {
    constexpr u32 SKELETAL_MESH_MAX_NUM_BONES = 100;
    return SKELETAL_MESH_MAX_NUM_BONES;
  }

  /**
   * @brief Creates a skeletal mesh from the specified file path.
   * This method loads and initializes a skeletal mesh object using the given absolute path.
   * It reads mesh and skeleton data from the file and prepares it for rendering and animation.
   *
   * @param absolute The absolute path to the skeletal mesh file.
   */
  void CreateFromFile(const fs::path& absolute);

  void Destroy();

  void Clone(SkeletalMesh& other) const;

  void Draw(RenderMode mode) const;
  
  /**
   * @brief Searches for a bone by its name.
   * @return A pair containing a pointer to the bone and its index, or {nullptr, -1} if the bone is not found.
   */
  std::pair<i32, Bone*> FindBone(StringView boneName) const;
  
  /** @brief This method calculates and returns the total count of vertices. */
  u32 TotalVertices() const;

  /** @brief This method calculates and returns the total count of indices. */
  u32 TotalIndices() const;

  u32 id;

  /** @brief A collection of meshes associated with the skeleton. */
  Mesh* meshes;
  u32 nrMeshes;

  /** @brief A collection of bones defining the skeleton. */
  Bone* bones;
  u32 nrBones;

  /** @brief Map storing the bone index with the name */
  struct BoneName
  {
    char name[32]{};
  };
  BoneName* boneNames;

  /** @brief The root node of the bone hierarchy. */
  BoneNode rootNode;

private:
  void ProcessNode(aiNode* node, const aiScene* scene);
  Buffer LoadVertices(aiMesh* aimesh);
  Buffer LoadIndices(aiMesh* aimesh);
  void LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh);
  void LoadBoneHierarchy(BoneNode& dest, const aiNode* src);
};