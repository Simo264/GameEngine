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
  SkeletalMesh() = default;

  /** @brief Default destructor for the SkeletonMesh class. */
  ~SkeletalMesh() = default;

  /** @brief Constructs a SkeletonMesh object by loading a skeletal model from the specified file path. */
  void CreateFromFile(const fs::path& path);

  /**
   * @brief Draws the skeleton using the specified rendering mode.
   * @param mode The rendering mode
   */
  void Draw(RenderMode mode);
  
  /**
   * @brief Searches for a bone by its name.
   * @param boneName The name of the bone to search for.
   * @return A pair containing a pointer to the bone and its index, or {nullptr, -1} if the bone is not found.
   */
  std::pair<const Bone*, i32> FindBone(StringView boneName) const;
  
  /**
   * @brief Inserts a new bone with the specified name or retrieves it if it already exists.
   * @param boneName The name of the bone to insert or retrieve.
   * @return A pair containing a pointer to the bone and its index.
   */
  std::pair<Bone*, i32> InsertBone(StringView boneName);

  /** @brief This method calculates and returns the total count of vertices. */
  u32 TotalVertices() const;

  /** @brief This method calculates and returns the total count of indices. */
  u32 TotalIndices() const;

  /** @brief A collection of meshes associated with the skeleton. */
  Vector<Mesh> meshes;

  /** @brief A collection of bones defining the skeleton. */
  Vector<Bone> bones;

  /** @brief The root node of the bone hierarchy. */
  BoneNode rootNode;

  UnorderedMap<String, u32> boneMap; // BoneName - boneIndex

  fs::path path;
  
private:
  void ProcessNode(aiNode* node, const aiScene* scene);
  Buffer LoadVertices(aiMesh* aimesh);
  Buffer LoadIndices(aiMesh* aimesh);
  Texture2D* GetMaterialTexture(aiMaterial* material, u32 textureType);
  void LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh);
  void LoadBoneHierarchy(BoneNode& dest, const aiNode* src);
};