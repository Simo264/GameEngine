#pragma once

#include <glm/vec2.hpp>               // vec2, bvec2, dvec2, ivec2 and uvec2
#include <glm/vec3.hpp>               // vec3, bvec3, dvec3, ivec3 and uvec3
#include <glm/vec4.hpp>               // vec4, bvec4, dvec4, ivec4 and uvec4
#include <glm/mat2x2.hpp>             // mat2, dmat2
#include <glm/mat2x3.hpp>             // mat2x3, dmat2x3
#include <glm/mat2x4.hpp>             // mat2x4, dmat2x4
#include <glm/mat3x2.hpp>             // mat3x2, dmat3x2
#include <glm/mat3x3.hpp>             // mat3, dmat3
#include <glm/mat3x4.hpp>             // mat3x4, dmat2
#include <glm/mat4x2.hpp>             // mat4x2, dmat4x2
#include <glm/mat4x3.hpp>             // mat4x3, dmat4x3
#include <glm/mat4x4.hpp>             // mat4, dmat4


/* -----------------------------
			GLM aliases
	 ----------------------------- */

using Vec2f = glm::vec2;		
using Vec2i = glm::ivec2;
using Vec2d = glm::dvec2;
using Vec2u = glm::uvec2;
using Vec3f = glm::vec3;		
using Vec3i = glm::ivec3;
using Vec3d = glm::dvec3;
using Vec3u = glm::uvec3;
using Vec4f = glm::vec4;		
using Vec4i = glm::ivec4;
using Vec4d = glm::dvec4;
using Vec4u = glm::uvec4;
using Mat2f = glm::mat2x2;	
using Mat2d = glm::dmat2x2;
using Mat3f = glm::mat3x3;	
using Mat3d = glm::dmat3x3;
using Mat4f = glm::mat4x4;	
using Mat4d = glm::dmat4x4;

namespace Math
{
	float Degrees(float radians);
	float Radians(float degrees);
	
	float Cos(float degree);
	float Sin(float degree);

	double Pow(double base, double exponent);

	Vec3f Cross(const Vec3f& v1, const Vec3f& v2);
	Vec3f Normalize(const Vec3f& v);

	float Length(const Vec3f& v);
}
