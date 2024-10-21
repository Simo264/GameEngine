#pragma once

#include "Core/Core.hpp"
#include "Core/GL.hpp"

enum class DepthFun : i32
{
	ALWAYS		= GL_ALWAYS,
	NEVER			= GL_NEVER,
	LESS			= GL_LESS,
	LEQUAL		= GL_LEQUAL,
	GREATER		= GL_GREATER,
	GEQUAL		= GL_GEQUAL,
	EQUAL			= GL_EQUAL,
	NOTEQUAL	= GL_NOTEQUAL
};

/**
 * https://open.gl/depthstencils
 * https://learnopengl.com/Advanced-OpenGL/Depth-testing
 *
 * ------------------------------------ Depth testing ------------------------------------
 * Z-buffering is a way of keeping track of the depth of every pixel on the screen.
 * The depth is an increasing function of the distance between the screen plane and a fragment that has been drawn.
 * That means that the fragments on the sides of the cube further away from the viewer have a higher depth value, whereas
 * fragments closer have a lower depth value.
 *
 * If this depth is stored along with the color when a fragment is written, fragments drawn later can compare
 * their depth to the existing depth to determine if the new fragment is closer to the viewer than the old fragment.
 * If that is the case, it should be drawn over and otherwise it can simply be discarded.
 * This is known as depth testing.
 *
 * OpenGL offers a way to store these depth values in an extra buffer, called the depth buffer,
 * and perform the required check for fragments automatically. This functionality can be enabled by calling glEnable.
 *
 * ------------------------------------ Depth test function ------------------------------------
 * OpenGL allows us to modify the comparison operators it uses for the depth test.
 * This allows us to control when OpenGL should pass or discard fragments and when to update the depth buffer
 *
 * By default the depth function GL_LESS is used that discards all the fragments that have a depth value higher
 * than or equal to the current depth buffer's value.
 */
namespace DepthTest 
{
	/**
	 * @brief
	 * Do depth comparisons and update the depth buffer
	 */
	void EnableTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	/**
	 * @brief
	 * Disable depth comparisons
	 */
	void DisableTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	/**
	 * @brief
	 * Enable writing into the depth buffer. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthMask.xhtml
	 */
	void EnableWritingBuffer()
	{
		glDepthMask(GL_TRUE);
	}

	/**
	 * @brief
	 * Disable writing into the depth buffer. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthMask.xhtml
	 */
	void DisableWritingBuffer()
	{
		glDepthMask(GL_FALSE);
	}

	/**
	 * @brief
	 * Specify the value used for depth buffer comparisons. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthFunc.xhtml
	 */
	void SetDepthFun(DepthFun fun)
	{
		glDepthFunc(static_cast<i32>(fun));
	}
}

