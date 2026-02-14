#pragma once

// Include only the specific GLM headers needed for quaternion types and operations
#include <glm/gtc/quaternion.hpp>  // For quaternion types and basic operations
#include <glm/gtc/constants.hpp>   // For mathematical constants (pi, etc.)

// Quaternion type aliases
using quatf= glm::quat;
// Double precision quaternions
using quatd = glm::dquat;

// Note: GLM's quaternion is defined in glm/gtc/quaternion.hpp
// It provides:
// - Basic quaternion operations (multiplication, conjugation, normalization)
// - Rotation between vectors
// - Euler angle conversions
// - Matrix conversions
// - Spherical linear interpolation (slerp)