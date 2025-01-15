#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Graphics/Objects/RenderBuffer.hpp"

enum class FramebufferTarget : u32 
{
	Draw			= 0x8CA9, // GL_DRAW_FRAMEBUFFER
	Read			= 0x8CA8, // GL_READ_FRAMEBUFFER
	ReadDraw	= 0x8D40  // GL_FRAMEBUFFER
};
enum class FramebufferAttachment : u32 
{
	Color0				= 0x8CE0, // GL_COLOR_ATTACHMENT0
	Color1				= 0x8CE1, // GL_COLOR_ATTACHMENT1
	Color2				= 0x8CE2, // GL_COLOR_ATTACHMENT2
	Color3				= 0x8CE3, // GL_COLOR_ATTACHMENT3
	Color4				= 0x8CE4, // GL_COLOR_ATTACHMENT4
	Color5				= 0x8CE5, // GL_COLOR_ATTACHMENT5
	Color6				= 0x8CE6, // GL_COLOR_ATTACHMENT6
	Color7				= 0x8CE7, // GL_COLOR_ATTACHMENT7
	Color8				= 0x8CE8, // GL_COLOR_ATTACHMENT8
	Color9				= 0x8CE9, // GL_COLOR_ATTACHMENT9
	// ...
	// Color31		= GL_COLOR_ATTACHMENT31
	Depth					= 0x8D00, // GL_DEPTH_ATTACHMENT
	Stencil				= 0x8D20, // GL_STENCIL_ATTACHMENT
	DepthStencil	= 0x821A  // GL_DEPTH_STENCIL_ATTACHMENT
};
enum class BlitFilter : u32 
{
	Nearest = 0x2600, // GL_NEAREST
	Linear	= 0x2601  // GL_LINEAR
};
enum class BlitMask : u32 
{
	ColorBuffer		= 0x00004000, // GL_COLOR_BUFFER_BIT
	DepthBuffer		= 0x00000100, // GL_DEPTH_BUFFER_BIT
	StencilBuffer = 0x00000400, // GL_STENCIL_BUFFER_BIT
	All						= ColorBuffer | DepthBuffer | StencilBuffer
};

/**
 * @brief
 * https://www.khronos.org/opengl/wiki/Framebuffer_Object
 * 
 * Framebuffer objects are a collection of attachments.
 * 
 * As standard OpenGL Objects, FBOs have the usual glGenFramebuffers and glDeleteFramebuffers functions. 
 * As expected, it also has the usual glBindFramebuffer function, to bind an FBO to the context.
 * 
 * The target​ parameter for this object can take one of 3 values: 
 * 1. GL_FRAMEBUFFER
 * 2. GL_READ_FRAMEBUFFER 
 * 3. GL_DRAW_FRAMEBUFFER
 * 
 * The last two allow you to bind an FBO so that reading commands (glReadPixels, etc) and writing commands 
 * (all rendering commands) can happen to two different framebuffers. 
 * The GL_FRAMEBUFFER binding target simply sets both the read and the write to the same FBO.
 *  
 * Each FBO image represents an attachment point, a location in the FBO where an image can be attached. 
 * FBOs have the following attachment points:
 * 	1. GL_COLOR_ATTACHMENTi:				these attachment points can only have images bound to them with color-renderable formats
 * 	2. GL_DEPTH_ATTACHMENT:					the image attached becomes the Depth Buffer for the FBO
 * 	3. GL_STENCIL_ATTACHMENT:				the image attached becomes the stencil buffer for the FBO
 * 	4. GL_DEPTH_STENCIL_ATTACHMENT:	the image attached becomes both the depth and stencil buffers
 *  
 * Renderbuffer objects contain images. They are created and used specifically with Framebuffer Objects. 
 * They are optimized for use as render targets, while Textures may not be, and are the logical choice 
 * when you do not need to sample from the produced image. 
 * If you need to resample, use Textures instead. 
 * Renderbuffer objects also natively accommodate Multisampling.
 */
class FrameBuffer
{
public:
	FrameBuffer() : 
		id{ 0 },
		textAttachments{},
		rboAttachments{}
	{}
	~FrameBuffer() = default;

	/**
	 * @brief
	 * Create framebuffer object
	 */
	void Create();

	/**
	 * @brief
	 * Delete framebuffer object and all attachments and invalidates the name associated with the frame buffer object 
	 */
	void Delete();

	/**
	 * @brief
	 * Bind the framebuffer to a framebuffer target
	 * 
	 * @param target: GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER
	 */
	void Bind(FramebufferTarget target) const;

	/**
	 * @brief
	 * Bind the framebuffer to a framebuffer target
	 *
	 * @param target: GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER
	 */
	void Unbind(FramebufferTarget target) const;

	/**
	 * @brief
	 * Check the completeness status of the framebuffer.
	 * 
	 * @return
	 * GL_FRAMEBUFFER_COMPLETE if the FBO can be used. If it is something else, then there is a problem.
	 */
	i32 CheckStatus() const;

	/**
	 * @brief
	 * Attach a level of a texture object as a logical buffer of the framebuffer object.
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFramebufferTexture.xhtml
	 */ 
	void AttachTexture(FramebufferAttachment attachment, u32 texture, i32 level);

	/**
	 * @brief
	 * Attach a renderbuffer as a logical buffer of the framebuffer object.
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFramebufferRenderbuffer.xhtml
	 */
	void AttachRenderBuffer(FramebufferAttachment attachment, RenderBuffer renderbuffer);

	/**
	 * @brief
	 * Copy a block of pixels from one framebuffer object to another.
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBlitFramebuffer.xhtml
	 */
	void Blit(const FrameBuffer& dest, 
		i32 srcLowerX,
		i32 srcLowerY,
		i32 srcUpperX,
		i32 srcUpperY,
		i32 destLowerX,
		i32 destLowerY,
		i32 destUpperX,
		i32 destUpperY,
		BlitMask mask,
		BlitFilter filter
	) const;

	/**
	 * @brief
	 * specify whether the individual color components in the frame buffer can or cannot be written.
	 * If red is GL_FALSE, for example, no change is made to the red component of any pixel in any of the color buffers, 
	 * regardless of the drawing operation attempted.
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glColorMask.xhtml
	 */
	void SetWritingColorComponents(bool r, bool g, bool b, bool a) const;

	bool IsValid() const;

	u32 id;

	/**
	 * @brief
	 * +16 textures for colour attachments
	 * +1 for the depth attachment (e.g., with GL_DEPTH_ATTACHMENT)
	 * +1 for the stencil attachment (e.g. with GL_STENCIL_ATTACHMENT).
	 */
	inline static constexpr i32 MAX_NUM_TEXTURE_ATTACHMENTS = 18;
	Vector<u32> textAttachments;

	/**
	 * @brief
	 * +16 renderbuffer for each of the 16 colour attachments
	 * +1 renderbuffer for the depth attachment.
	 * +1 renderbuffer for the stencil attachment.
	 */
	inline static constexpr i32 MAX_NUM_RBO_ATTACHMENTS = 18;
	Vector<u32> rboAttachments;
};
