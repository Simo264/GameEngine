#pragma once

#include "Core/Core.hpp"

enum class CompareFunc : u32
{
	Never = 0x0200, // GL_NEVER
	Less = 0x0201, // GL_LESS
	Equal = 0x0202, // GL_EQUAL
	LEqual = 0x0203, // GL_LEQUAL
	Greater = 0x0204, // GL_GREATER
	NEqual = 0x0205, // GL_NOTEQUAL
	GEqual = 0x0206, // GL_GEQUAL
	Always = 0x0207  // GL_ALWAYS
};

enum class CullFaceMode : u32
{
	Front = 0x0404, // GL_FRONT
	Back = 0x0405, // GL_BACK
	FrontAndBack = 0x0408, // GL_FRONT_AND_BACK
};

enum class FrontFaceMode : u32
{
	CCW = 0x0901, // GL_CCW
	CW = 0x0900,	// GL_CW
};

enum class StencilOpMode : u32
{
	Keep = 0x1E00, // GL_KEEP
	Zero = 0x0000, // GL_ZERO
	Replace = 0x1E01, // GL_REPLACE
	Increment = 0x1E02, // GL_INCR
	IncrementWrap = 0x8507, // GL_INCR_WRAP
	Decrement = 0x1E03, // GL_DECR
	DecrementWrap = 0x8508, // GL_DECR_WRAP
	Invert = 0x150A  // GL_INVERT
};
