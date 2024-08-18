#pragma once

#include "Core/Core.hpp"

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

namespace Stencil
{
	/**
	 * Enable stencil testing and update the stencil buffer
	 */
	void EnableTest();

	/**
	 * Disable stencil testing
	 */
	void DisableTest();

	/**
	 * Specifies the conditions under which a fragment passes the stencil test.
	 *
	 * @param func: the test function; can be
	 *							GL_NEVER, GL_LESS, GL_LEQUAL, GL_GREATER, GL_GEQUAL, GL_EQUAL, GL_NOTEQUAL, and GL_ALWAYS
	 *
	 * @param ref:	a value to compare the stencil value to using the test function
	 *
	 * @param mask:	a bitwise AND operation is performed on the stencil value and reference value with this mask value
	 *							before comparing them
	 *  */
	void SetFunction(i32 func, i32 ref, i32 mask);


	/**
	 * Specifies what should happen to stencil values depending on the outcome of the stencil and depth tests.
	 * Eight symbolic constants are accepted: GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR,
	 *																				GL_DECR_WRAP, and GL_INVERT
	 *
	 * @param sfail:	action to take if the stencil test fails
	 * @param dpfail: action to take if the stencil test is successful, but the depth test failed
	 * @param dppass: action to take if both the stencil test and depth tests pass
	 */
	void SetOperation(i32 sfail, i32 dpfail, i32 dppass);


	/**
	 * Specifies a bit mask to enable and disable writing of individual bits in the stencil planes.
	 * Where a 1 appears in the mask, it's possible to write to the corresponding bit in the stencil buffer. 
	 * Where a 0 appears, the corresponding bit is write-protected
	 *
	 * @param mask: specifies a bit mask to enable and disable writing of individual bits in the stencil planes
	 */
	void SetMask(i32 mask);


	void EnableWritingBuffer();

	void DisableWritingBuffer();
}

