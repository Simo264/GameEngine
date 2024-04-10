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
	 * Delete framebuffer object and all attachments
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
	 * Check the completeness status of the framebuffer
	 */
	bool CheckStatus() const;

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
	void AttachTexture(int attachment, Texture2D& texture, int level);

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
	void AttachRenderBuffer(int attachment, RenderBuffer& renderbuffer);

	constexpr bool IsValid()	const { return id != static_cast<uint32_t>(-1); }
	//constexpr int GetWidth()	const { return _size.x; }
	//constexpr int GetHeigth() const { return _size.y; }

	constexpr Texture2D& GetTextureAttachment(int i) { return _textAttachments.at(i); }
	constexpr RenderBuffer& GetRenderBufferAttachment(int i) { return _rboAttachments.at(i); }

	constexpr int GetNumTextureAttachments() const { return _textAttachments.size(); }
	constexpr int GetNumRenderBufferAttachments() const { return _rboAttachments.size(); }

#if 0
	FrameBuffer(const Vec2i32& size);
	
	~FrameBuffer();


	//void Create();

	/* 
		By binding to the GL_FRAMEBUFFER target all the next read 
		and write framebuffer operations will affect the currently bound framebuffer. 
		It is also possible to bind a framebuffer to a read or write target specifically 
		by calling to BindRead() or BindWrite(). 
	*/
	void Bind() const;
	void Unbind() const;
	
	/* 
		The framebuffer bound to GL_READ_FRAMEBUFFER is then used for all read operations 
		like glReadPixels 
	*/
	void BindRead() const;
	void UnbindRead() const;

	/* 
		The framebuffer bound to GL_DRAW_FRAMEBUFFER is used as the destination for rendering, 
		clearing and other write operations 
	*/
	void BindWrite() const;
	void UnbindWrite() const;
	
	/* 
		A multisampled image contains much more information than a normal image so 
		what we need to do is downscale or resolve the image. 
		Resolving a multisampled framebuffer is generally done through glBlitFramebuffer that
		copies a region from one framebuffer to the other while also resolving any multisampled buffers 
	*/
	void Blit() const;
	
	

	//void SetPostProcessing(PostProcessingType type) { _postprocType = type; }
	
	/* Resize the frame buffer viewport */
	void Rescale(const Vec2i32& size);

	/* Update multisampled buffers with new sample value */
	void SetSamples(int samples);

	/* Return the texture color attachment */
	constexpr uint32_t GetImage() const { return _colorAttachment; }

	constexpr Vec2i32 GetSize() const { return _size; }

	constexpr float GetAspect() const { return static_cast<float>(_size.x) / static_cast<float>(_size.y); }

	constexpr int GetSamples() const { return _samples; }
#endif

	uint32_t id;

private:
	Vector<Texture2D>			_textAttachments;	/* vector of all attached texture ids */
	Vector<RenderBuffer>	_rboAttachments;	/* vector of all attached renderbuffer ids */

	//Vec2i32 _size; /* the framebuffer size (width and height) */

#if 0
	Vec2i32 _size;

	int _samples;				/* number of samples for multisampling */

	uint32_t _fbo;							/* primary frame buffer object */
	uint32_t _intermediateFbo;	/* second post-processing framebuffer */
	
	uint32_t _colorAttachment;						/* Texture object */
	uint32_t _colorAttachmentMultisampled;/* Texture object */
	
	uint32_t _depthStencilAttachmentMultisampled; /* Renderbuffer object */
	
	/* Attach color to framebuffer using texture */
	void ColorAttachment();

	/* Attach both multisampled depth and stencil using renderbuffer */
	void DepthStencilAttachment();

	bool CheckStatus() const;

	void UpdateTexture(uint32_t texture, int width, int height);
	void UpdateMultisampledTexture(uint32_t texture, int samples, int width, int height);
	void UpdateMultisampledRenderbuffer(uint32_t renderbuffer, int samples, int width, int height);
#endif
};
