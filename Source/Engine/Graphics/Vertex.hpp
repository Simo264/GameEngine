#pragma once

#include "Core/Math/Base.hpp"

/**
 * @struct VertexFormat
 * @brief Defines the structure of a vertex attribute in a vertex buffer.
 */
struct VertexFormat
{
  /** @brief Number of components per vertex attribute. Must be 1, 2, 3, 4, or GL_BGRA.*/
  i32 size{ 0 };

  /** @brief Specifies the data type of the attribute. https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribFormat.xhtml */
  i32 type{ 0 };

  /**
   * @brief Specifies if integer data should be normalized to [0, 1] or [-1, 1].
   * If true, signed/unsigned integers are normalized. If false, data is converted directly to float.
   */
  bool normalized{ false };

  /** @brief Offset of the attribute relative to the start of the vertex buffer. */
  i32 relativeoffset{ 0 };
};

/**
 * @struct Vertex_P
 * @brief Vertex format containing only position.
 */
struct Vertex_P
{
  /** @brief Vertex position (x, y, z). */
	vec3f position{};
};

/**
 * @struct Vertex_P_UV
 * @brief Vertex format containing position and UV coordinates.
 */
struct Vertex_P_UV
{
  /** @brief Vertex position (x, y, z). */
	vec3f position{};
  /** @brief Texture UV coordinates (u, v). */
	vec2f uv{};
};

/**
 * @struct Vertex_P_N_UV_T
 * @brief Vertex format containing position, normal, UV coordinates, and tangent.
 */
struct Vertex_P_N_UV_T
{
  /** @brief Vertex position (x, y, z). */
  vec3f position{};
  /** @brief Vertex normal vector (x, y, z). */
  vec3f normal{};
  /** @brief Texture UV coordinates (u, v). */
  vec2f uv{};
  /** @brief Tangent vector (x, y, z). */
  vec3f tangent{};
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
  vec3f position{};
  /** @brief Vertex normal vector (x, y, z). */
  vec3f normal{};
  /** @brief Texture UV coordinates (u, v). */
  vec2f uv{};
  /** @brief Tangent vector (x, y, z). */
  vec3f tangent{};

  /** @brief IDs of bones affecting the vertex. */
  i32 boneIds[MAX_BONES_INFLUENCE];
  /** @brief Weights from each bone. */
  f32 boneWeights[MAX_BONES_INFLUENCE];
};
