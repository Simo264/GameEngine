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
	 * Delete renderbuffer objects and invalidates the name associated with the render buffer object 
	 */
	void Delete();

	/**
	 * Establish data storage, format and dimensions of the renderbuffer object's image
	 * 
	 * @param width:	specifies the width of the renderbuffer, in pixels.
	 * 
	 * @param height: specifies the height of the renderbuffer, in pixels.
	 */
	void CreateStorage(int width, int height);

	/**
	 * Establish data storage, format, dimensions and sample count of the renderbuffer object's image
	 * 
	 * @param samples:	specifies the number of samples to be used for the renderbuffer object's storage.
	 * 
	 * @param width:		specifies the width of the renderbuffer, in pixels.
	 * 
	 * @param height:		specifies the height of the renderbuffer, in pixels.
	 */
	void CreateStorageMulstisampled(int samples, int width, int height);

	constexpr bool IsMultisampled() const { return _isMultisampled; }
	
	constexpr int GetSamples() const { return _samples; }

	uint32_t id;

	/**
	 * Specifies the internal format to be used for the renderbuffer object's storage and must be a 
	 * color-renderable, depth-renderable, or stencil-renderable format.
	 * 
	 * https://www.khronos.org/opengl/wiki/Image_Format
	 */
	int internalformat;

private:
	int  _samples;
	bool _isMultisampled;
};
