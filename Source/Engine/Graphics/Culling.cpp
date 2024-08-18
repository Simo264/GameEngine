#include "Culling.hpp"

#include "Core/OpenGL.hpp"

namespace Culling
{
	void EnableFaceCulling()
	{
		glEnable(GL_CULL_FACE);
	}

	void DisableFaceCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	void SetCullFace(i32 mode)
	{
		glCullFace(mode);
	}

	void SetFrontFace(i32 mode)
	{
		glFrontFace(mode);
	}
}