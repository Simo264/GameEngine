#pragma once

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
 * 
 */

namespace Depth 
{
	/**
 * Do depth comparisons and update the depth buffer
 */
	void EnableTest();

	/**
	 * Disable depth comparisons
	 */
	void DisableTest();

	/**
	 * Enable writing into the depth buffer
	 */
	void EnableWritingBuffer();

	/**
	 * Disable writing into the depth buffer
	 */
	void DisableWritingBuffer();

	/**
		* Specify the value used for depth buffer comparisons
		*
		* @param func: GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, and GL_ALWAYS
		*/
	void SetFunction(int func);
}

