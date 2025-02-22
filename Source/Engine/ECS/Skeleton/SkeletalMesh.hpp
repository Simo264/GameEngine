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

/** @brief Represents a skeletal mesh for rendering and manipulation of skeleton-based models. */
class SkeletalMesh
{
public:
  SkeletalMesh() :
    rootNode{},
    meshes{},
    bones{},
    boneNames{},
    nrBones{ 0 },
    nrMeshes{ 0 },
    id{ 0 }
  {}

  /**
   * @brief Releases CPU-side memory used by the skeletal mesh.
   *
   * This destructor frees the allocated memory for the `meshes`, `bones`, and `boneNames` arrays.
   * It does not affect GPU resources, as those must be explicitly released using `Destroy()`
   * before deleting the object.
   *
   * @note This destructor is called when a `SkeletalMesh` component is removed from an object
   *       or when the original instance is destroyed by `ModelsManager`.
   */
  ~SkeletalMesh() = default;

  /** @brief Move constructor */
  SkeletalMesh(SkeletalMesh&&) noexcept = default;
  SkeletalMesh& operator=(SkeletalMesh&&) noexcept = default;

  /** @brief Delete copy constructor */
  SkeletalMesh(const SkeletalMesh&) = delete;
  SkeletalMesh& operator=(const SkeletalMesh&) = delete;

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

  /**
   * @brief Creates a copy of the current skeletal mesh, including meshes, bones, and bone names.
   *
   * This method allocates new memory for the `Mesh`, `Bone`, and `BoneName` arrays in the target
   * `SkeletalMesh` (`other`), ensuring that each copy has its own independent data. The VAOs (vertex
   * array objects) and GPU resources are shared between copies.
   *
   * @note Since VAOs and GPU buffers are shared across copies, changes to the mesh geometry will affect
   *       all copies. The bones and bone names, however, are copied independently.
   */
  void Clone(SkeletalMesh& other) const;
  
  /**
   * @brief Frees GPU resources by destroying OpenGL buffers and VAOs for all meshes.
   *
   * This method releases all OpenGL-related resources, including vertex array objects (VAOs)
   * and associated buffers, for each mesh in the skeletal mesh. It should only be called
   * on the original instance to avoid affecting shared meshes.
   *
   * @note Since VAOs and buffers are shared across copies, calling `Destroy()` on a copy
   *       would result in invalid references. Ensure that only the original instance calls this method.
   */
  void Destroy() const;

  void Draw(RenderMode mode) const;
  
  /**
   * @brief Searches for a bone by its name.
   * @return A pair containing a pointer to the bone and its index, or {nullptr, -1} if the bone is not found.
   */
  std::pair<i32, Bone*> FindBone(StringView boneName) const;
  
  u32 TotalVertices() const;

  u32 TotalIndices() const;

  /** @brief The root node of the bone hierarchy. */
  UniquePtr<BoneNode> rootNode;

  /** @brief List of meshes associated with the skeleton. */
  UniquePtr<Mesh[]> meshes;
  u32 nrMeshes;

  /** @brief A collection of bones defining the skeleton. */
  UniquePtr<Bone[]> bones;
  u32 nrBones;

  /** @brief Map storing the bone index with the name */
  struct BoneName
  {
    char name[32]{};
  };
  UniquePtr<BoneName[]> boneNames;

  u32 id;

private:
  void ProcessNode(aiNode* node, const aiScene* scene);
  Buffer LoadVertices(aiMesh* aimesh);
  Buffer LoadIndices(aiMesh* aimesh);
  void LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh);
  void LoadBoneHierarchy(BoneNode& dest, const aiNode* src);
};