#include "Stencil.hpp"

#include "Core/OpenGL.hpp"

namespace Stencil
{
	void EnableTest()
	{
		glEnable(GL_STENCIL_TEST);
	}

	void DisableTest()
	{
		glDisable(GL_STENCIL_TEST);
	}

	void SetFunction(int func, int ref, int mask)
	{
		glStencilFunc(func, ref, mask);
	}

	void SetOperation(int sfail, int dpfail, int dppass)
	{
		glStencilOp(sfail, dpfail, dppass);
	}

	void SetMask(int mask)
	{
		glStencilMask(mask);
	}

	void EnableWritingBuffer()
	{
		SetMask(0XFF);
	}

	void DisableWritingBuffer()
	{
		SetMask(0X00);
	}
}

