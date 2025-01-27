#pragma once

#include "Core/Core.hpp"


enum class RenderbufferInternalFormat : u32
{
	R8 = 0x8229, // GL_R8
	RG8 = 0x822B, // GL_RG8
	RGB8 = 0x8051, // GL_RGB8
	RGBA8 = 0x8058, // GL_RGBA8
	RGB10_A2 = 0x8059, // GL_RGB10_A2
	R16F = 0x822D, // GL_R16F
	RG16F = 0x822F, // GL_RG16F
	RGB16F = 0x881B, // GL_RGB16F
	RGBA16F = 0x881A, // GL_RGBA16F
	R32F = 0x822E, // GL_R32F
	RG32F = 0x8230, // GL_RG32F
	RGB32F = 0x8815, // GL_RGB32F
	RGBA32F = 0x8814, // GL_RGBA32F

	DepthComponent16 = 0x81A5, // GL_DEPTH_COMPONENT16
	DepthComponent24 = 0x81A6, // GL_DEPTH_COMPONENT24
	DepthComponent32F = 0x8CAC, // GL_DEPTH_COMPONENT32F
	Depth24Stencil8 = 0x88F0, // GL_DEPTH24_STENCIL8
	Depth32FStencil8 = 0x8CAD, // GL_DEPTH32F_STENCIL8
	StencilIndex8 = 0x8D48  // GL_STENCIL_INDEX8
};

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
	void CreateStorage(RenderbufferInternalFormat internalFormat, i32 width, i32 height) const;

	/**
	 * @brief
	 * Establish data storage, format, dimensions and sample count of the renderbuffer object's image.
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glRenderbufferStorageMultisample.xhtml
	 */
	void CreateStorageMulstisampled(RenderbufferInternalFormat internalFormat, i32 samples, i32 width, i32 height) const;

	bool IsValid() const;

	u32 id;
};
