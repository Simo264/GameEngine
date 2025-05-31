#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

/**
 * @struct Vertex_P
 * @brief Vertex format containing only position.
 */
struct Vertex_P
{
  /** @brief Vertex position (x, y, z). */
	Vec3F position{};
};

/**
 * @struct Vertex_P_UV
 * @brief Vertex format containing position and UV coordinates.
 */
struct Vertex_P_UV
{
  /** @brief Vertex position (x, y, z). */
	Vec3F position{};
  /** @brief Texture UV coordinates (u, v). */
	Vec2F uv{};
};

/**
 * @struct Vertex_P_N_UV_T
 * @brief Vertex format containing position, normal, UV coordinates, and tangent.
 */
struct Vertex_P_N_UV_T
{
  /** @brief Vertex position (x, y, z). */
  Vec3F position{};
  /** @brief Vertex normal vector (x, y, z). */
  Vec3F normal{};
  /** @brief Texture UV coordinates (u, v). */
  Vec2F uv{};
  /** @brief Tangent vector (x, y, z). */
  Vec3F tangent{};
};

/**
 * @struct Vertex_P_N_UV_T_B
 * @brief Vertex format containing position, normal, UV coordinates, tangent, and bone influences.
 */
struct Vertex_P_N_UV_T_B 
{
  Vertex_P_N_UV_T_B();

  /**
   * @brief Adds a bone influence to the vertex.
   *
   * @param id ID of the bone.
   * @param weight Weight of the bone's influence.
   */
  void AddBone(i32 id, f32 weight);


  /** @brief Maximum number of bones influencing a single vertex. */
  static constexpr u32 MAX_BONES_INFLUENCE = 4;

  /** @brief Vertex position (x, y, z). */
  Vec3F position{};
  /** @brief Vertex normal vector (x, y, z). */
  Vec3F normal{};
  /** @brief Texture UV coordinates (u, v). */
  Vec2F uv{};
  /** @brief Tangent vector (x, y, z). */
  Vec3F tangent{};

  /** @brief IDs of bones affecting the vertex. */
  i32 boneIds[MAX_BONES_INFLUENCE];
  /** @brief Weights from each bone. */
  f32 boneWeights[MAX_BONES_INFLUENCE];
};
