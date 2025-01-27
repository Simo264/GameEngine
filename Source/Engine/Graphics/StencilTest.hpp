#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/OpenGLEnums.hpp"

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
 * What happens to those values can be controlled by you using the glStencilFunc, glStencilOp and glStencilMask functions.
 */

namespace StencilTest
{
	/** @brief Enable stencil testing and update the stencil buffer */
	void EnableTest();

	/** @brief Disable stencil testing */
	void DisableTest();

	/** @brief Specifies the conditions under which a fragment passes the stencil test. */
	void SetStencilFun(CompareFunc fun, i32 ref, i32 mask);

	/** @brief Specifies what should happen to stencil values depending on the outcome of the stencil and depth tests. */
	void SetStencilOp(StencilOpMode sfail, StencilOpMode dpfail, StencilOpMode dppass);

	/** @brief Specifies a bit mask to enable and disable writing of individual bits in the stencil planes. */
	void SetStencilMask(i32 mask);

	void EnableWritingBuffer();

	void DisableWritingBuffer();
}