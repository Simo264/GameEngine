#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>  // For vector operations like dot, cross, normalize, length, distance
#include <glm/common.hpp>     // For min, max, clamp, etc.
#include <glm/exponential.hpp> // For pow, exp, log, sqrt, etc.
#include <glm/trigonometric.hpp> // For sin, cos, tan, etc.

// Float vectors
using vec2f = glm::vec2;
using vec3f = glm::vec3;
using vec4f = glm::vec4;

// Double precision vectors
using vec2d = glm::dvec2;
using vec3d = glm::dvec3;
using vec4d = glm::dvec4;

// Integer vectors (32-bit)
using vec2i = glm::ivec2;
using vec3i = glm::ivec3;
using vec4i = glm::ivec4;

// Unsigned integer vectors (32-bit)
using vec2u = glm::uvec2;
using vec3u = glm::uvec3;
using vec4u = glm::uvec4;

// Boolean vectors
using vec2b = glm::bvec2;
using vec3b = glm::bvec3;
using vec4b = glm::bvec4;
