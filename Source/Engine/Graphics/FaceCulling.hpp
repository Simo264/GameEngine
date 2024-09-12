#pragma once

#include "Core/Core.hpp"
#include "Core/GL.hpp"

/**
 * https://learnopengl.com/Advanced-OpenGL/Face-culling
 * 
 * OpenGL checks all the faces that are front facing towards the viewer and renders those while discarding 
 * all the faces that are back facing, saving us a lot of fragment shader calls.
 * We do need to tell OpenGL which of the faces we use are actually the front faces and which faces are the back faces. 
 * OpenGL uses a clever trick for this by analyzing the winding order of the vertex data.
 * 
 * When we define a set of triangle vertices we're defining them in a certain winding order that is either 
 * clockwise or counter-clockwise.
 * Each set of 3 vertices that form a triangle primitive thus contain a winding order. 
 * OpenGL uses this information when rendering your primitives to determine if a triangle is a front-facing or a 
 * back-facing triangle.
 * 
 * OpenGL is able to discard triangle primitives if they're rendered as back-facing triangles.
 * To enable face culling we only have to enable OpenGL's GL_CULL_FACE option: glEnable(GL_CULL_FACE);  
 * From this point on, all the faces that are not front-faces are discarded.
 * Currently we save over 50% of performance on rendering fragments if OpenGL decides to render the back faces first.
 * 
 * Do note that this only really works with closed shapes like a cube. We do have to disable face culling again 
 * when we draw the grass leaves, since their front and back face should be visible.
 * 
 */

enum class CullFace : i32
{
	FRONT = GL_FRONT,
	BACK = GL_BACK,
	FRONT_BACK = GL_FRONT_AND_BACK
};

enum class FrontFace : i32
{
	CLOCKWISE = GL_CW,
	COUNTER_CLOCKWISE = GL_CCW
};

namespace FaceCulling
{
	/**
	 * Cull polygons based on their winding in window coordinates
	 */
	void EnableFaceCulling()
	{
		glEnable(GL_CULL_FACE);
	}

	/**
	 * Disable face culling
	 */
	void DisableFaceCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	/**
	 * Specify whether front- or back-facing facets can be culled.
	 */
	void SetCullFace(CullFace mode)
	{
		glCullFace(static_cast<i32>(mode));
	}

	/**
	 * Specifies which of the clockwise and counterclockwise facets are front-facing and back-facing
	 */
	void SetFrontFacing(FrontFace mode)
	{
		glFrontFace(static_cast<i32>(mode));
	}
}
