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
   * @brief Creates a SkeletalMesh from a file located at the given path and allocates GPU resources.
   * This method imports a 3D model containing skeletal information from the specified file using the Assimp library.
   *
   * The method allocates memory for the meshes and bones, and it creates GPU resources (VAO, vertex buffers,
   * element buffers) for each mesh. The bone data is loaded and stored in a hierarchy structure.
   *
   * - Allocates memory for the meshes (each mesh having its own VAO and buffers for vertices and indices).
   * - Allocates memory for the bones and their names.
   * - Creates GPU resources for each mesh, including vertex and element buffers.
   * - Builds the bone hierarchy by recursively processing the nodes in the model.
   *
   * Additionally, if the model has associated textures (e.g., diffuse, specular, normal maps), these
   * textures are loaded and assigned to the corresponding materials.
   *
   * @param absolute The absolute path to the 3D model file to load.
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
   * @brief Finds the index of a bone given its name.
   * This method searches for a bone in the mesh's bone list by comparing the provided name with the stored bone names.
   * If a match is found, the index of the bone in the bone list is returned. If no bone with the specified name is found,
   * the method returns -1.
   *
   * The bone names are stored in an array with a maximum length of 32 characters. This ensures that the bone names
   * do not exceed the specified size.
   *
   * @return The index of the bone if found, otherwise -1.
   */
  i32 FindBone(StringView boneName) const;
  
  u32 TotalVertices() const;

  u32 TotalIndices() const;

  /**
   * @brief The root bone node of the bone hierarchy.
   *
   * This shared pointer ensures that all instances of the same skeletal
   * mesh share the same bone hierarchy, optimizing memory usage by
   * preventing the creation of redundant bone hierarchies for the same
   * skeleton.
   */
  SharedPtr<BoneNode> rootNode;
  
  /**
   * @brief Array of bones associated with the skeletal mesh.
   *
   * This array holds all the bones that influence the skeletal mesh,
   * where each bone corresponds to a specific part of the mesh. The array
   * is shared across all instances of the skeletal mesh, ensuring that
   * the same set of bones is used.
   */
  SharedPtr<Bone[]> bones;
  
  /**
   * @brief Array of bone names, corresponding to the bones in the 'bones' array.
   *
   * This array stores the names of each bone. It is parallel to the 'bones'
   * array, meaning that each element in 'boneNames' corresponds to the
   * bone at the same index in the 'bones' array. 
   * The array is shared across all instances of the skeletal mesh, ensuring that
   * the same set of boneNames is used.
   */
  SharedPtr<Array<char, 32>[]> boneNames;

  /**
   * @brief Array of meshes that compose this skeletal mesh.
   * This is a unique pointer because each mesh can have different materials and must
   * be treated as a distinct entity. Each `Mesh` within this array is unique to the
   * skeletal mesh instance.
   */
  UniquePtr<Mesh[]> meshes;
  
  u32 nrMeshes;

  u32 nrBones;

  /** @brief Unique identifier referencing the original SkeletalMesh stored in ModelsManager. */
  u32 id;

private:
  void ProcessNode(aiNode* node, const aiScene* scene);
  Buffer LoadVertices(aiMesh* aimesh);
  Buffer LoadIndices(aiMesh* aimesh);
  void LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh);
  void LoadBoneHierarchy(BoneNode& dest, const aiNode* src);
};