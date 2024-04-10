#pragma once

#include "Core/Core.hpp"

/**
 *	https://www.khronos.org/opengl/wiki/Renderbuffer_Object
 *	
 *	Renderbuffer Objects are OpenGL Objects that contain images. 
 *	They are created and used specifically with Framebuffer Objects. 
 *	They are optimized for use as render targets, while Textures may not be, and are the logical choice when you do not need 
 *	to sample (i.e. in a post-pass shader) from the produced image. 
 *	If you need to resample (such as when reading depth back in a second shader pass), use Textures instead. 
 *	Renderbuffer objects also natively accommodate Multisampling (MSAA).
 */
class RenderBuffer
{
public:
	RenderBuffer();

	~RenderBuffer() = default;

	/**
	 * Create renderbuffer objects
	 */
	void Create();

	/**
	 * Delete renderbuffer objects
	 */
	void Delete();

	/**
	 * Establish data storage, format and dimensions of a renderbuffer object's image
	 * 
	 * @param internalformat: specifies the internal format to use for the renderbuffer object's image
	 * @param width:					specifies the width of the renderbuffer, in pixels.
	 * @param height:					specifies the height of the renderbuffer, in pixels.
	 */
	void CreateStorage(int internalformat, int width, int height) const;

	uint32_t id;
};
