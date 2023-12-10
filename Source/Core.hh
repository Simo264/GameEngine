#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <sstream>
#include <exception>
#include <vector>

using std::string;
using std::array;
using std::vector;
using std::unique_ptr;
using std::stringstream;
using std::ifstream;

using vec2f = glm::vec2;
using vec2i = glm::ivec2;
using vec2d = glm::dvec2;
using vec2u = glm::uvec2;

using vec3f = glm::vec3;
using vec3i = glm::ivec3;
using vec3d = glm::dvec3;
using vec3u = glm::uvec3;

using vec4f = glm::vec4;
using vec4i = glm::ivec4;
using vec4d = glm::dvec4;
using vec4u = glm::uvec4;

using mat2f = glm::mat2x2;
using mat2i = glm::imat2x2;
using mat2d = glm::dmat2x2;
using mat2u = glm::umat2x2;

using mat3f = glm::mat3x3;
using mat3i = glm::imat3x3;
using mat3d = glm::dmat3x3;
using mat3u = glm::umat3x3;

using mat4f = glm::mat4x4;
using mat4i = glm::imat4x4;
using mat4d = glm::dmat4x4;
using mat4u = glm::umat4x4;