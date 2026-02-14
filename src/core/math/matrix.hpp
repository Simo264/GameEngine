#pragma once

// Include only the specific GLM headers needed for matrix types and operations
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>        // For matrix operations: transpose, inverse, determinant, etc.
#include <glm/gtc/matrix_transform.hpp> // For matrix transformations: translate, rotate, scale, lookAt, ortho, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp>   // For inverse matrix operations

// Matrix type aliases
// Float matrices (most commonly used)
using mat2 = glm::mat2x2;
using mat3 = glm::mat3x3;
using mat4 = glm::mat4x4;

// Double precision matrices
using mat2d = glm::dmat2x2;
using mat3d = glm::dmat3x3;
using mat4d = glm::dmat4x4;
