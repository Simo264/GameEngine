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

using Vec2f			= glm::vec2;		/* 2 components vector of floating-point numbers */
using Vec2d			= glm::dvec2;		/* 2 components vector of double-precision numbers */
using Vec2i8		= glm::i8vec2;	/* 2 components vector of 8 bit integer numbers */
using Vec2i16		= glm::i16vec2;	/* 2 components vector of 16 bit integer numbers */
using Vec2i32		= glm::i32vec2;	/* 2 components vector of 32 bit integer numbers */
using Vec2i64		= glm::i64vec2;	/* 2 components vector of 64 bit integer numbers */
using Vec2ui8		= glm::u8vec2;	/* 2 components vector of 8 bit unsigned integer numbers */
using Vec2ui16	= glm::u16vec2;	/* 2 components vector of 16 bit unsigned integer numbers */
using Vec2ui32	= glm::u32vec2;	/* 2 components vector of 32 bit unsigned integer numbers */
using Vec2ui64	= glm::u64vec2;	/* 2 components vector of 64 bit unsigned integer numbers */

using Vec3f			= glm::vec3;		/* 3 components vector of floating-point numbers */
using Vec3d			= glm::dvec3;		/* 3 components vector of double-precision numbers */
using Vec3i8		= glm::i8vec3;	/* 3 components vector of 8 bit integer numbers */
using Vec3i16		= glm::i16vec3;	/* 3 components vector of 16 bit integer numbers */
using Vec3i32		= glm::i32vec3;	/* 3 components vector of 32 bit integer numbers */
using Vec3i64		= glm::i64vec3;	/* 3 components vector of 64 bit integer numbers */
using Vec3ui8		= glm::u8vec3;	/* 3 components vector of 8 bit unsigned integer numbers */
using Vec3ui16	= glm::u16vec3;	/* 3 components vector of 16 bit unsigned integer numbers */
using Vec3ui32	= glm::u32vec3;	/* 3 components vector of 32 bit unsigned integer numbers */
using Vec3ui64	= glm::u64vec3;	/* 3 components vector of 64 bit unsigned integer numbers */

using Vec4f			= glm::vec4;		/* 4 components vector of floating-point numbers */
using Vec4d			= glm::dvec4;		/* 4 components vector of double-precision numbers */
using Vec4i8		= glm::i8vec4;	/* 4 components vector of 8 bit integer numbers */
using Vec4i16		= glm::i16vec4;	/* 4 components vector of 16 bit integer numbers */
using Vec4i32		= glm::i32vec4;	/* 4 components vector of 32 bit integer numbers */
using Vec4i64		= glm::i64vec4;	/* 4 components vector of 64 bit integer numbers */
using Vec4ui8		= glm::u8vec4;	/* 4 components vector of 8 bit unsigned integer numbers */
using Vec4ui16	= glm::u16vec4;	/* 4 components vector of 16 bit unsigned integer numbers */
using Vec4ui32	= glm::u32vec4;	/* 4 components vector of 32 bit unsigned integer numbers */
using Vec4ui64	= glm::u64vec4;	/* 4 components vector of 64 bit unsigned integer numbers */

using Mat2f = glm::mat2x2;	/* 2x2 matrix of floating-point numbers */
using Mat2d = glm::dmat2x2;	/* 2x2 matrix of double-precision numbers */
using Mat3f = glm::mat3x3;	/* 3x3 matrix of floating-point numbers */
using Mat3d = glm::dmat3x3;	/* 3x3 matrix of double-precision numbers */
using Mat4f = glm::mat4x4;	/* 4x4 matrix of floating-point numbers */
using Mat4d = glm::dmat4x4;	/* 4x4 matrix of double-precision numbers */

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
