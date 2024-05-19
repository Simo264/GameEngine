#include "Depth.hpp"

#include "Core/OpenGL.hpp"

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
	void SetFunction(int func)
	{
		glDepthFunc(func);
	}
}


