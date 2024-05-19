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

	void SetCullFace(int mode)
	{
		glCullFace(mode);
	}

	void SetFrontFace(int mode)
	{
		glFrontFace(mode);
	}
}