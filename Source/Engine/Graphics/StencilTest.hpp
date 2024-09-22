#pragma once

#include "Core/Core.hpp"
#include "Core/GL.hpp"

/**
 * https://open.gl/depthstencils
 * 
 * ----------------------  Stencil buffer ---------------------- 
 * The stencil buffer is an optional extension of the depth buffer that gives you more control over the question 
 * of which fragments should be drawn and which shouldn't. 
 * Like the depth buffer, a value is stored for every pixel, but this time you get to control when and how 
 * this value changes and when a fragment should be drawn depending on this value.
 * 
 * Note that if the depth test fails, the stencil test no longer determines whether a fragment is drawn or not, 
 * but these fragments can still affect values in the stencil buffer!
 * 
 * ----------------------  Stencil testing ---------------------- 
 * Stencil testing is enabled with a call to glEnable, just like depth testing.
 * 
 * Regular drawing operations are used to determine which values in the stencil buffer are affected by any stencil 
 * operation. If you want to affect a rectangle of values like in the sample above, simply draw a 2D quad in that area.
 * 
 * 
 * What happens to those values can be controlled by you using the glStencilFunc, glStencilOp and glStencilMask functions.
 * 
 */

enum class StencilFun : i32
{
	ALWAYS = GL_ALWAYS,
	NEVER = GL_NEVER,
	LESS = GL_LESS,
	LEQUAL = GL_LEQUAL,
	GREATER = GL_GREATER,
	GEQUAL = GL_GEQUAL,
	EQUAL = GL_EQUAL,
	NOTEQUAL = GL_NOTEQUAL
};

enum class StencilOp : i32 
{
	KEEP = GL_KEEP,
	INVERT = GL_INVERT,
	ZERO = GL_ZERO,
	REPLACE = GL_REPLACE,
	INCR = GL_INCR,
	INCR_WRAP = GL_INCR_WRAP,
	DECR = GL_DECR,
	DECR_WRAP = GL_DECR_WRAP
};

namespace StencilTest
{
	/**
	 * Enable stencil testing and update the stencil buffer
	 */
	void EnableTest()
	{
		glEnable(GL_STENCIL_TEST);
	}

	/**
	 * Disable stencil testing
	 */
	void DisableTest()
	{
		glDisable(GL_STENCIL_TEST);
	}

	/**
	 * Specifies the conditions under which a fragment passes the stencil test.
	 *
	 * @param ref:	a value to compare the stencil value to using the test function. The initial value is 0
	 *
	 * @param mask:	a bitwise AND operation is performed on the stencil value and reference value with this mask value
	 *							before comparing them. The initial value is all 1's.
	 *  */
	void SetStencilFun(StencilFun fun, i32 ref, i32 mask)
	{
		glStencilFunc(static_cast<i32>(fun), ref, mask);
	}

	/**
	 * Specifies what should happen to stencil values depending on the outcome of the stencil and depth tests.
	 * 
	 * @param sfail:	action to take if the stencil test fails. The initial value is KEEP
	 * @param dpfail: action to take if the stencil test is successful, but the depth test failed. The initial value is KEEP
	 * @param dppass: action to take if both the stencil test and depth tests pass. The initial value is KEEP
	 */
	void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass)
	{
		glStencilOp(static_cast<i32>(sfail), static_cast<i32>(dpfail), static_cast<i32>(dppass));
	}

	/**
	 * Specifies a bit mask to enable and disable writing of individual bits in the stencil planes.
	 * Where a 1 appears in the mask, it's possible to write to the corresponding bit in the stencil buffer. 
	 * Where a 0 appears, the corresponding bit is write-protected
	 *
	 * @param mask: specifies a bit mask to enable and disable writing of individual bits in the stencil planes
	 */
	void SetStencilMask(i32 mask)
	{
		glStencilMask(mask);
	}

	void EnableWritingBuffer()
	{
		SetStencilMask(0xFF);
	}

	void DisableWritingBuffer()
	{
		SetStencilMask(0x00);
	}
}

