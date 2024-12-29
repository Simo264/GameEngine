#pragma once

#include "Core/Math/Core.hpp"

struct VertexFormat
{
  /**
   * The number of components per vertex are allocated to the specified attribute and must be 1, 2, 3, 4, or GL_BGRA
   */
  i32 size{ 0 };

  /**
   * Indicates the type of the data. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribFormat.xhtml
   */
  i32 type{ 0 };

  /**
   * If true then integer data is normalized to the range [-1, 1] or [0, 1] if it is signed or unsigned, respectively.
   * Otherwise if normalized is false then integer data is directly converted to floating point
   */
  bool normalized{ false };

  /**
   * Is the offset of the first element relative to the start of the vertex buffer binding this attribute fetches from
   */
  i32 relativeoffset{ 0 };
};

/**
 * @brief
 * Vertex format with position
 */
struct Vertex_P
{
	vec3f position{};
};

/**
 * @brief
 * Vertex format with position and uv coordinates
 */
struct Vertex_P_UV
{
	vec3f position{};
	vec2f uv{};
};

/**
 * @brief
 * Vertex format with position, normal, uv coordinates and tangent
 */
struct Vertex_P_N_UV_T
{
  vec3f position{};
  vec3f normal{};
  vec2f uv{};
  vec3f tangent{};
};

/**
 * @brief
 * Vertex format with position, normal, uv coordinates, tangent and bones
 */
struct Vertex_P_N_UV_T_B 
{
  /* One vertex can be affected by 4 bones */
  static constexpr u32 MAX_BONES_INFLUENCE = 4;

  vec3f position{};
  vec3f normal{};
  vec2f uv{};
  vec3f tangent{};

  /* Joints which will influence this vertex */
  i32 boneIds[MAX_BONES_INFLUENCE] = { -1 };
  /* Weights from each bone */
  f32 boneWeights[MAX_BONES_INFLUENCE] = { 0.f };

  void AddBone(u32 id, f32 weight);
};
