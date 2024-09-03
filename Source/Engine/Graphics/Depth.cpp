#include "Depth.hpp"

#include "Core/GL.hpp"

namespace Depth
{
	void EnableTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void DisableTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void EnableWritingBuffer()
	{
		glDepthMask(GL_TRUE);
	}

	void DisableWritingBuffer()
	{
		glDepthMask(GL_FALSE);
	}
	void SetFunction(i32 func)
	{
		glDepthFunc(func);
	}
}


