#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/Graphics/Mesh.hpp"
#include "Engine/ECS/Skeleton/Bone.hpp"

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
class SkeletonMesh
{
public:
  /**
   * @brief Constructs a SkeletonMesh object by loading a skeletal model from the specified file path.
   * @param skeletalPath The file path to the skeletal model.
   */
  SkeletonMesh(const fs::path& skeletalPath);
  
  /** @brief Default destructor for the SkeletonMesh class. */
  ~SkeletonMesh() = default;

  /**
   * @brief Draws the skeleton using the specified rendering mode.
   * @param mode The rendering mode
   */
  void Draw(i32 mode);
  
  /**
   * @brief Searches for a bone by its name.
   * @param boneName The name of the bone to search for.
   * @return A pair containing a pointer to the bone and its index, or {nullptr, -1} if the bone is not found.
   */
  std::pair<Bone*, u32> FindBone(StringView boneName);
  
  /**
   * @brief Inserts a new bone with the specified name or retrieves it if it already exists.
   * @param boneName The name of the bone to insert or retrieve.
   * @return A pair containing a pointer to the bone and its index.
   */
  std::pair<Bone*, u32> InsertBone(StringView boneName);

  u32 TotalVertices() const;
  u32 TotalIndices() const;

  /** @brief The file path of the skeletal model. */
  fs::path path;

  /** @brief A collection of meshes associated with the skeleton. */
  Vector<Mesh> meshes;

  /** @brief A collection of bones defining the skeleton. */
  Vector<Bone> bones;

  /** @brief The root node of the bone hierarchy. */
  BoneNode rootNode;
  
private:
  void ProcessNode(aiNode* node, const aiScene* scene);
  Buffer LoadVertices(aiMesh* aimesh);
  Buffer LoadIndices(aiMesh* aimesh);
  Texture2D* GetMaterialTexture(aiMaterial* material, u32 textureType);
  void LoadBonesAndWeights(Vector<Vertex_P_N_UV_T_B>& vertices, const aiMesh* aimesh);
  void LoadBoneHierarchy(BoneNode& dest, const aiNode* src);

  UnorderedMap<String, u32> _boneMap; // BoneName - boneIndex
};
