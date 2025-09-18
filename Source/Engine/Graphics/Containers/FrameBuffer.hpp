#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Graphics/Objects/Renderbuffer.hpp"

enum class FramebufferTarget : u32
{
	DRAW = 0x8CA9, // GL_DRAW_FRAMEBUFFER
	READ = 0x8CA8, // GL_READ_FRAMEBUFFER
	READ_DRAW = 0x8D40  // GL_FRAMEBUFFER
};

enum class FramebufferAttachment : u32
{
	COLOR_ATTACHMENT0 = 0x8CE0, // GL_COLOR_ATTACHMENT0
	COLOR_ATTACHMENT1 = 0x8CE1, // GL_COLOR_ATTACHMENT1
	COLOR_ATTACHMENT2 = 0x8CE2, // GL_COLOR_ATTACHMENT2
	COLOR_ATTACHMENT3 = 0x8CE3, // GL_COLOR_ATTACHMENT3
	COLOR_ATTACHMENT4 = 0x8CE4, // GL_COLOR_ATTACHMENT4
	COLOR_ATTACHMENT5 = 0x8CE5, // GL_COLOR_ATTACHMENT5
	COLOR_ATTACHMENT6 = 0x8CE6, // GL_COLOR_ATTACHMENT6
	COLOR_ATTACHMENT7 = 0x8CE7, // GL_COLOR_ATTACHMENT7
	COLOR_ATTACHMENT8 = 0x8CE8, // GL_COLOR_ATTACHMENT8
	COLOR_ATTACHMENT9 = 0x8CE9, // GL_COLOR_ATTACHMENT9
	// ...
	// Color31		= GL_COLOR_ATTACHMENT31
	DEPTH = 0x8D00, // GL_DEPTH_ATTACHMENT
	STENCIL = 0x8D20, // GL_STENCIL_ATTACHMENT
	DEPTH_STENCIL = 0x821A  // GL_DEPTH_STENCIL_ATTACHMENT
};

enum class FramebufferBlitFilter : u32
{
	NEAREST = 0x2600, // GL_NEAREST
	LINEAR = 0x2601  // GL_LINEAR
};

enum class FramebufferBlitMask : u32
{
	COLOR_BUFFER = 0x00004000, // GL_COLOR_BUFFER_BIT
	DEPTH_BUFFER = 0x00000100, // GL_DEPTH_BUFFER_BIT
	STENCIL_BUFFER = 0x00000400, // GL_STENCIL_BUFFER_BIT
	DEPTH_STENCIL_BUFFER = 0x00000100 | 0x00000400 
};

enum class FramebufferStatus : i32
{
	COMPLETE = 0x8CD5,                 // GL_FRAMEBUFFER_COMPLETE
	UNDEFINED = 0x8219,                // GL_FRAMEBUFFER_UNDEFINED
	INCOMPLETE_ATTACHMENT = 0x8CD6,     // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
	INCOMPLETE_MISSING_ATTACHMENT = 0x8CD7, // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
	INCOMPLETE_DRAW_BUFFER = 0x8CDB,     // GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
	INCOMPLETE_READ_BUFFER = 0x8CDC,     // GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
	UNSUPPORTED = 0x8CDD,              // GL_FRAMEBUFFER_UNSUPPORTED
	INCOMPLETE_MULTISAMPLE = 0x8D56,    // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
	INCOMPLETE_LAYER_TARGETS = 0x8DA8    // GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
};

enum class FramebufferDrawBuffer : u32
{
	NONE = 0x0000,              // GL_NONE
	FRONT_LEFT = 0x0400,        // GL_FRONT_LEFT
	FRONT_RIGHT = 0x0401,       // GL_FRONT_RIGHT
	BACK_LEFT = 0x0402,         // GL_BACK_LEFT
	BACK_RIGHT = 0x0403,        // GL_BACK_RIGHT
	COLOR_ATTACHMENT0 = 0x8CE0, // GL_COLOR_ATTACHMENT0
	COLOR_ATTACHMENT1 = 0x8CE1, // GL_COLOR_ATTACHMENT1
	COLOR_ATTACHMENT2 = 0x8CE2, // GL_COLOR_ATTACHMENT2
	COLOR_ATTACHMENT3 = 0x8CE3, // GL_COLOR_ATTACHMENT3
	COLOR_ATTACHMENT4 = 0x8CE4, // GL_COLOR_ATTACHMENT4
	COLOR_ATTACHMENT5 = 0x8CE5, // GL_COLOR_ATTACHMENT5
	COLOR_ATTACHMENT6 = 0x8CE6, // GL_COLOR_ATTACHMENT6
	COLOR_ATTACHMENT7 = 0x8CE7, // GL_COLOR_ATTACHMENT7
	COLOR_ATTACHMENT8 = 0x8CE8, // GL_COLOR_ATTACHMENT8
};

/**
 * @brief Framebuffer objects are a collection of attachments.
 * https://www.khronos.org/opengl/wiki/Framebuffer_Object
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
class Framebuffer
{
public:
	Framebuffer() : 
		id{ 0u },
		textAttachments{},
		rboAttachments{}
	{}
	~Framebuffer() = default;

	/** @brief Create framebuffer object */
	void Create();

	/** @brief Delete framebuffer object and all attachments */
	void Delete();

	/** @brief Bind the framebuffer to a framebuffer target */
	void Bind(FramebufferTarget target) const;

	/** @brief Bind the framebuffer to a framebuffer target */
	void Unbind(FramebufferTarget target) const;

	/** @brief Check the completeness status of the framebuffer. */
	FramebufferStatus CheckStatus() const;

	/** @brief Attach a level of a texture object as a logical buffer of the framebuffer object. */ 
	void AttachTexture(FramebufferAttachment attachment, Texture2D texture, i32 level);

	/** @brief Attach a renderbuffer as a logical buffer of the framebuffer object. */
	void AttachRenderBuffer(FramebufferAttachment attachment, Renderbuffer renderbuffer);

	/** @brief Copy a block of pixels from one framebuffer object to another. */
	void Blit(const Framebuffer& dest, 
						i32 srcLowerX,
						i32 srcLowerY,
						i32 srcUpperX,
						i32 srcUpperY,
						i32 destLowerX,
						i32 destLowerY,
						i32 destUpperX,
						i32 destUpperY,
						FramebufferBlitMask mask,
						FramebufferBlitFilter filter) const;

	/**
	 * @brief Specify whether the individual color components in the frame buffer can or cannot be written.
	 * If red is GL_FALSE, for example, no change is made to the red component of any pixel in any of the color buffers, 
	 * regardless of the drawing operation attempted.
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glColorMask.xhtml
	 */
	void SetWritingColorComponents(bool r, bool g, bool b, bool a) const;

	/**
	 * @brief Specifies a list of color buffers to be drawn into
	 * 
	 * @param n Specifies the number of buffers in buffers.
	 * @param buffers Points to an array of symbolic constants specifying the buffers into which 
	 *								fragment colors or data values will be written.
	 */
	void SetDrawBuffers(u32 n, FramebufferDrawBuffer bufs[]) const;

	bool IsValid() const;

	Texture2D GetTextureAttachment(u32 index);
	Renderbuffer GetRenderbufferAttachment(u32 index);

	/**
	 * @brief
	 * +16 textures for color attachments
	 * +1 for the depth attachment (e.g., with GL_DEPTH_ATTACHMENT)
	 * +1 for the stencil attachment (e.g. with GL_STENCIL_ATTACHMENT).
	 */
	inline static constexpr i32 MAX_NUM_TEXTURE_ATTACHMENTS = 18;
	Vector<Texture2D> textAttachments;

	/**
	 * @brief
	 * +16 renderbuffer for each of the 16 colour attachments
	 * +1 renderbuffer for the depth attachment.
	 * +1 renderbuffer for the stencil attachment.
	 */
	inline static constexpr i32 MAX_NUM_RBO_ATTACHMENTS = 18;
	Vector<Renderbuffer> rboAttachments;

	u32 id;
};
