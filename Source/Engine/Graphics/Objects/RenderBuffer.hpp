#pragma once

#include "Core/Core.hpp"

/**
 * @brief
 * https://www.khronos.org/opengl/wiki/Renderbuffer_Object
 * 
 * Renderbuffer Objects are OpenGL Objects that contain images. 
 * They are created and used specifically with Framebuffer Objects. 
 * They are optimized for use as render targets, while Textures may not be, and are the logical choice when you do not need 
 * to sample (i.e. in a post-pass shader) from the produced image. 
 * If you need to resample (such as when reading depth back in a second shader pass), use Textures instead. 
 * Renderbuffer objects also natively accommodate Multisampling (MSAA).
 */
class RenderBuffer
{
public:
	RenderBuffer() : 
		id{ 0 } 
	{}
	~RenderBuffer() = default;

	/**
	 * @brief
	 * Create renderbuffer objects
	 */
	void Create();

	/**
	 * @brief
	 * Delete renderbuffer objects and invalidates the name associated with the render buffer object 
	 */
	void Delete();

	/**
	 * @brief
	 * Establish data storage, format and dimensions of the renderbuffer object's image.
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glRenderbufferStorage.xhtml
	 */
	void CreateStorage(i32 internalFormat, i32 width, i32 height) const;

	/**
	 * @brief
	 * Establish data storage, format, dimensions and sample count of the renderbuffer object's image.
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glRenderbufferStorageMultisample.xhtml
	 */
	void CreateStorageMulstisampled(i32 internalFormat, i32 samples, i32 width, i32 height) const;

	bool IsValid() const;

	u32 id;
};
