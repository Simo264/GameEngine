#include "Math.hpp"

#include <glm/common.hpp>             // all the GLSL common functions: abs, min, mix, isnan, fma, etc.
#include <glm/exponential.hpp>        // all the GLSL exponential functions: pow, log, exp2, sqrt, etc.
#include <glm/geometric.hpp>          // all the GLSL geometry functions: dot, cross, reflect, etc.
#include <glm/integer.hpp>            // all the GLSL integer functions: findMSB, bitfieldExtract, etc.
#include <glm/matrix.hpp>             // all the GLSL matrix functions: transpose, inverse, etc.
#include <glm/packing.hpp>            // all the GLSL packing functions: packUnorm4x8, unpackHalf2x16, etc.
#include <glm/trigonometric.hpp>      // all the GLSL trigonometric functions: radians, cos, asin, etc.
#include <glm/vector_relational.hpp>  // all the GLSL vector relational functions: equal, less, etc.

namespace Math
{
	float Degrees(float radians)
	{
		return glm::degrees(radians);
	}
	
	float Radians(float degrees)
	{
		return glm::radians(degrees);
	}

	float Cos(float degree)
	{
		return glm::cos(degree);
	}

	float Sin(float degree)
	{
		return glm::sin(degree);
	}

	double Pow(double base, double exponent)
	{
		return glm::pow(base, exponent);
	}

	vec3f Cross(const vec3f& v1, const vec3f& v2)
	{
		return glm::cross(v1, v2);
	}

	vec3f Normalize(const vec3f& v)
	{
		return glm::normalize(v);
	}

	float Length(const vec3f& v)
	{
		return glm::length(v);
	}
}
