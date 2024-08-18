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

	void SetFunction(i32 func, i32 ref, i32 mask)
	{
		glStencilFunc(func, ref, mask);
	}

	void SetOperation(i32 sfail, i32 dpfail, i32 dppass)
	{
		glStencilOp(sfail, dpfail, dppass);
	}

	void SetMask(i32 mask)
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

