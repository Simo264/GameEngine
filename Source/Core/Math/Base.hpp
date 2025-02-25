#pragma once

#include "Core/Core.hpp"

// Include all GLM core / GLSL features
#include <glm/glm.hpp>						// glm::vec2, glm::vec3, glm::mat4, glm::radians
#include <glm/gtc/quaternion.hpp> // glm::quat

/** ----------------------------------------------------------
 *												GLM aliases
 * ---------------------------------------------------------- */

using vec2f	= glm::vec2;
using vec2d	= glm::dvec2;
using vec2i	= glm::i32vec2;
using vec2u	= glm::u32vec2;

using vec3f	= glm::vec3;
using vec3d	= glm::dvec3;
using vec3i	= glm::i32vec3;
using vec3u	= glm::u32vec3;

using vec4f	= glm::vec4;
using vec4d	= glm::dvec4;
using vec4i	= glm::i32vec4;
using vec4u	= glm::u32vec4;

using mat2f = glm::mat2x2;
using mat2d = glm::dmat2x2;
using mat3f = glm::mat3x3;
using mat3d = glm::dmat3x3;
using mat4f = glm::mat4x4;
using mat4d = glm::dmat4x4;

using quat	= glm::quat;