#include "StencilTest.hpp"
#include "Core/GL.hpp"

namespace StencilTest
{
	/** @brief Enable stencil testing and update the stencil buffer */
	void EnableTest()
	{
		glEnable(GL_STENCIL_TEST);
	}

	/** @brief Disable stencil testing */
	void DisableTest()
	{
		glDisable(GL_STENCIL_TEST);
	}

	/** @brief Specifies the conditions under which a fragment passes the stencil test. */
	void SetStencilFun(CompareFunc fun, i32 ref, i32 mask)
	{
		glStencilFunc(static_cast<u32>(fun), ref, mask);
	}

	/** @brief Specifies what should happen to stencil values depending on the outcome of the stencil and depth tests. */
	void SetStencilOp(StencilOpMode sfail, StencilOpMode dpfail, StencilOpMode dppass)
	{
		glStencilOp(static_cast<i32>(sfail), static_cast<i32>(dpfail), static_cast<i32>(dppass));
	}

	/** @brief Specifies a bit mask to enable and disable writing of individual bits in the stencil planes. */
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