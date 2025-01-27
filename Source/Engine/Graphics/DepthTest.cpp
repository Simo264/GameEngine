#include "DepthTest.hpp"

#include "Core/GL.hpp"

namespace DepthTest
{
	/** @brief Do depth comparisons and update the depth buffer */
	void EnableTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	/** @brief Disable depth comparisons */
	void DisableTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	/** @brief Enable writing into the depth buffer. */
	void EnableWritingBuffer()
	{
		glDepthMask(GL_TRUE);
	}

	/** @brief Disable writing into the depth buffer. */
	void DisableWritingBuffer()
	{
		glDepthMask(GL_FALSE);
	}

	/** @brief Specify the value used for depth buffer comparisons. */
	void SetDepthFun(CompareFunc fun)
	{
		glDepthFunc(static_cast<u32>(fun));
	}
}

