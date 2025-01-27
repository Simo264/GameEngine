#include "FaceCulling.hpp"

#include "Core/GL.hpp"

namespace FaceCulling
{
	/** @brief Cull polygons based on their winding in window coordinates */
	void EnableFaceCulling()
	{
		glEnable(GL_CULL_FACE);
	}

	/** @brief Disable face culling */
	void DisableFaceCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	/** @brief Specify whether front- or back-facing facets can be culled. */
	void SetCullFace(CullFaceMode mode)
	{
		glCullFace(static_cast<u32>(mode));
	}

	/** @brief Specifies which of the clockwise and counterclockwise facets are front-facing and back-facing. */
	void SetFrontFacing(FrontFaceMode mode)
	{
		glFrontFace(static_cast<u32>(mode));
	}
}
