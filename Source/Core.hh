#pragma once

#include <Glad/gl.h>
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
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>

/* -----------------------------
			STL library aliases
   ----------------------------- */

template<typename type, size_t size> 
using Array					= std::array<type, size>;
template<typename type>
using Vector				= std::vector<type>;
template<typename typeName, typename typeVal>
using Map						= std::map<typeName, typeVal>;

template<typename type>
using UniquePointer = std::unique_ptr<type>;
template<typename type>
using SharedPointer = std::shared_ptr<type>;

using String				= std::string;
using StringStream	= std::stringstream;
using IStringStream = std::istringstream;
using OStringStream = std::ostringstream;
using IFStream			= std::ifstream;
using OFStream			= std::ofstream;
using RuntimeError	= std::runtime_error;
using Path					= std::filesystem::path;

const Path ROOT_PATH = std::filesystem::current_path().parent_path();

/* -----------------------------
			GLM library aliases
	 ----------------------------- */

using Vec2f = glm::vec2;		/* Vec2 */
using Vec2i = glm::ivec2;
using Vec2d = glm::dvec2;
using Vec2u = glm::uvec2;
using Vec3f = glm::vec3;		/* Vec3 */
using Vec3i = glm::ivec3;
using Vec3d = glm::dvec3;
using Vec3u = glm::uvec3;
using Vec4f = glm::vec4;		/* Vec4 */
using Vec4i = glm::ivec4;
using Vec4d = glm::dvec4;
using Vec4u = glm::uvec4;
using Mat2f = glm::mat2x2;	/* Mat2 */ 
using Mat2i = glm::imat2x2;
using Mat2d = glm::dmat2x2;
using Mat2u = glm::umat2x2;
using Mat3f = glm::mat3x3;	/* Mat3 */ 
using Mat3i = glm::imat3x3;
using Mat3d = glm::dmat3x3;
using Mat3u = glm::umat3x3;
using Mat4f = glm::mat4x4;	/* Mat4 */ 
using Mat4i = glm::imat4x4;
using Mat4d = glm::dmat4x4;
using Mat4u = glm::umat4x4;