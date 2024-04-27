#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/RenderBuffer.hpp"

enum class PostProcessingType : int {
	POST_PROC_NONE			= 0,
	POST_PROC_INVERSION = 1,
	POST_PROC_GRAYSCALE = 2,
	POST_PROC_KERNEL		= 3,
	POST_PROC_BLUR			= 4,
};

//struct TextureAttachment
//{
//	Texture2D texture;
//	int attachment;
//	int index;
//};
//
//struct RenderbufferAttachment
//{
//	RenderBuffer renderbuffer;
//	int attachment;
//	int index;
//};

/**
 *	https://www.khronos.org/opengl/wiki/Framebuffer_Object
 * 
 *  Framebuffer objects are a collection of attachments.
 *	
 *  As standard OpenGL Objects, FBOs have the usual glGenFramebuffers and glDeleteFramebuffers functions. 
 *  As expected, it also has the usual glBindFramebuffer function, to bind an FBO to the context.
 * 
 *  The target​ parameter for this object can take one of 3 values: 
 *		1. GL_FRAMEBUFFER
 *		2. GL_READ_FRAMEBUFFER 
 *		3. GL_DRAW_FRAMEBUFFER
 * 
 *	The last two allow you to bind an FBO so that reading commands (glReadPixels, etc) and writing commands 
 *	(all rendering commands) can happen to two different framebuffers. 
 *	The GL_FRAMEBUFFER binding target simply sets both the read and the write to the same FBO.
 * 
 *	Each FBO image represents an attachment point, a location in the FBO where an image can be attached. 
 *	FBOs have the following attachment points:
 *		1. GL_COLOR_ATTACHMENTi:				these attachment points can only have images bound to them with color-renderable formats
 *		2. GL_DEPTH_ATTACHMENT:					the image attached becomes the Depth Buffer for the FBO
 *		3. GL_STENCIL_ATTACHMENT:				the image attached becomes the stencil buffer for the FBO
 *		4. GL_DEPTH_STENCIL_ATTACHMENT:	the image attached becomes both the depth and stencil buffers
 * 
 *	Renderbuffer objects contain images. They are created and used specifically with Framebuffer Objects. 
 *	They are optimized for use as render targets, while Textures may not be, and are the logical choice 
 *	when you do not need to sample from the produced image. 
 *	If you need to resample, use Textures instead. 
 *	Renderbuffer objects also natively accommodate Multisampling.
 */
class FrameBuffer
{
public:
	FrameBuffer();

	~FrameBuffer() = default;

	/**
	 * Create framebuffer object
	 */
	void Create();

	/**
	 * Delete framebuffer object and all attachments and invalidates the name associated with the frame buffer object 
	 */
	void Delete();

	/**
	 * Bind the framebuffer to a framebuffer target
	 * 
	 * @param target: specifies the framebuffer target of the binding operation, must be either 
	 *								GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER
	 *								
	 */
	void Bind(int target) const;

	/**
	 * Bind the framebuffer to a framebuffer target
	 *
	 * @param target: specifies the framebuffer target of the binding operation, must be either 
	 *								GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER
	 */
	void Unbind(int target) const;

	/**
	 * Check the completeness status of the framebuffer.
	 * The return value is GL_FRAMEBUFFER_COMPLETE if the FBO can be used. If it is something else, then there is a problem.
	 * 
	 * 
	 * 
	 */
	int CheckStatus() const;

	/**
	 * Attach a level of a texture object as a logical buffer of the framebuffer object
	 * 
	 * @param attachment: specifies the logical attachment of the framebuffer and must be 
	 *										GL_COLOR_ATTACHMENTi, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT or GL_DEPTH_STENCIL_ATTACHMENT.
	 *										Attaching a level of a texture to GL_DEPTH_STENCIL_ATTACHMENT is equivalent to attaching that level 
	 *										to both the GL_DEPTH_ATTACHMENT and the GL_STENCIL_ATTACHMENT attachment points simultaneously
	 * 
	 * @param texture:		specifies the name of an existing texture object to attach.
	 *										if texture is the name of a three-dimensional, cube map array, cube map, one- or two-dimensional array, 
	 *										or two-dimensional multisample array texture, the specified texture level is an array of images, 
	 *										and the framebuffer attachment is considered to be layeredif texture is the name of a three-dimensional, 
	 *										cube map array, cube map, one- or two-dimensional array, or two-dimensional multisample array texture, 
	 *										the specified texture level is an array of images, and the framebuffer attachment is considered 
	 *										to be layered
	 * 
	 * @param level:			specifies the mipmap level of the texture object to attach
	 */ 
	void AttachTexture(int attachment, const Texture2D& texture, int level);

	/**
	 * Attach a renderbuffer as a logical buffer of the framebuffer object
	 * 
	 * @param attachment:		specifies the logical attachment of the framebuffer and must be 
	 *											GL_COLOR_ATTACHMENTi, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT or GL_DEPTH_STENCIL_ATTACHMENT.
	 *											Setting attachment to the value GL_DEPTH_STENCIL_ATTACHMENT is a special case causing both the 
	 *											depth and stencil attachments of the specified framebuffer object to be set to renderbuffer, 
	 *											which should have the base internal format GL_DEPTH_STENCIL.
	 * 
	 * @param renderbuffer: specifies the name of an existing renderbuffer object of type renderbuffertarget to attach
	 */
	void AttachRenderBuffer(int attachment, const RenderBuffer& renderbuffer);

	/**
	 * Copy a block of pixels from one framebuffer object to another.
	 * 
	 * @param dest: specifies the name of the destination framebuffer object
	 * 
	 * @param srcLowerX, 
	 *				srcLowerY, 
	 *				destLowerX, 
	 *				destLowerY: specify the bounds of the source rectangle within the read buffer of the read framebuffer.
	 * 
	 * @param srcUpperX,
	 * 				srcUpperY,
	 *				destUpperX,
	 * 				destUpperY: specify the bounds of the destination rectangle within the write buffer of the write framebuffer.
	 * 
	 * @param mask:				the bitwise OR of the flags indicating which buffers are to be copied. 
	 *										The allowed flags are GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT.
	 * 
	 * @param filter:			specifies the interpolation to be applied if the image is stretched. 
	 *										Must be GL_NEAREST or GL_LINEAR.
	 * 
	 */
	void Blit(
		const FrameBuffer& dest, 
		int srcLowerX,
		int srcLowerY,
		int srcUpperX,
		int srcUpperY,
		int destLowerX,
		int destLowerY,
		int destUpperX,
		int destUpperY,
		int mask,
		int filter
	) const;

	constexpr bool IsValid()	const { return id != static_cast<uint32_t>(-1); }

	constexpr Texture2D& GetTextureAttachment(int i) { return _textAttachments.at(i); }
	constexpr RenderBuffer& GetRenderBufferAttachment(int i) { return _rboAttachments.at(i); }

	constexpr int GetNumTextureAttachments() const { return _textAttachments.size(); }
	constexpr int GetNumRenderBufferAttachments() const { return _rboAttachments.size(); }

	uint32_t id;

private:
	vector<Texture2D>			_textAttachments;	/* vector of all attached texture ids */
	vector<RenderBuffer>	_rboAttachments;	/* vector of all attached renderbuffer ids */
};
