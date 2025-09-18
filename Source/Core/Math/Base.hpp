#pragma once

#include <glm/glm.hpp>						// glm::vec2, glm::vec3, glm::mat4, glm::radians
#include <glm/gtc/quaternion.hpp> // glm::quat

/** ----------------------------------------------------------
 *												GLM aliases
 * ---------------------------------------------------------- */

using Vec2I = glm::i32vec2;
using Vec2F	= glm::vec2;
using Vec2D = glm::f64vec2;

using Vec3I = glm::i32vec3;
using Vec3F	= glm::vec3;

using Vec4I = glm::i32vec4;
using Vec4F	= glm::vec4;

using Mat2F = glm::mat2x2;
using Mat3F = glm::mat3x3;
using Mat4F = glm::mat4x4;

using Quat	= glm::quat;