#include "FrameBuffer.hpp"
#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

void FrameBuffer::Create()
{
	glCreateFramebuffers(1, &id);
}

void FrameBuffer::Delete()
{
	u64 size = textAttachments.size();
	if (size > 0)
		glDeleteTextures(size, textAttachments.data());

	size = rboAttachments.size();
	if (size > 0)
		glDeleteRenderbuffers(size, rboAttachments.data());

	glDeleteFramebuffers(1, &id);
	id = 0;
	textAttachments.clear();
	rboAttachments.clear();
}

void FrameBuffer::Bind(FramebufferTarget target) const
{
	glBindFramebuffer(static_cast<u32>(target), id);
}

void FrameBuffer::Unbind(FramebufferTarget target) const
{
	glBindFramebuffer(static_cast<u32>(target), 0);
}

i32 FrameBuffer::CheckStatus() const
{
	return glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
}

bool FrameBuffer::IsValid() const
{
	return (id != 0) && (glIsFramebuffer(id) == GL_TRUE);
}

void FrameBuffer::AttachTexture(FramebufferAttachment attachment, u32 texture, i32 level)
{
	if (textAttachments.size() >= MAX_NUM_TEXTURE_ATTACHMENTS)
	{
		CONSOLE_WARN("Cannot add more texture attachments. Maximum limit of {} reached.", MAX_NUM_TEXTURE_ATTACHMENTS);
		return;
	}

	textAttachments.push_back(texture);
	glNamedFramebufferTexture(id, static_cast<u32>(attachment), texture, level);
}

void FrameBuffer::AttachRenderBuffer(FramebufferAttachment attachment, RenderBuffer renderbuffer)
{
	if (rboAttachments.size() >= MAX_NUM_RBO_ATTACHMENTS)
	{
		CONSOLE_WARN("Cannot add more renderbuffer attachments. Maximum limit of {} reached.", MAX_NUM_RBO_ATTACHMENTS);
		return;
	}

	rboAttachments.push_back(renderbuffer.id);
	glNamedFramebufferRenderbuffer(id, static_cast<u32>(attachment), GL_RENDERBUFFER, renderbuffer.id);
}

void FrameBuffer::Blit(
	const FrameBuffer& dest,
	i32 srcLowerX,
	i32 srcLowerY,
	i32 srcUpperX,
	i32 srcUpperY,
	i32 destLowerX,
	i32 destLowerY,
	i32 destUpperX,
	i32 destUpperY,
	FramebufferBlitMask mask,
	FramebufferBlitFilter filter) const
{
	glBlitNamedFramebuffer(
		id, dest.id, 
		srcLowerX, srcLowerY, 
		srcUpperX, srcUpperY, 
		destLowerX, destLowerY, 
		destUpperX, destUpperY, 
		static_cast<u32>(mask),
		static_cast<u32>(filter));
}

void FrameBuffer::SetWritingColorComponents(bool r, bool g, bool b, bool a) const
{
	glColorMaski(id, r, g, b, a);
}