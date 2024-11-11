#include "FrameBuffer.hpp"
#include "Core/GL.hpp"

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
		glDeleteTextures(size, rboAttachments.data());

	glDeleteFramebuffers(1, &id);
	id = 0;
	textAttachments.clear();
	rboAttachments.clear();
}

void FrameBuffer::Bind(i32 target) const
{
	glBindFramebuffer(target, id);
}

void FrameBuffer::Unbind(i32 target) const
{
	glBindFramebuffer(target, 0);
}

i32 FrameBuffer::CheckStatus() const
{
	return glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
}

bool FrameBuffer::IsValid() const
{
	return (id != 0) && (glIsFramebuffer(id) == GL_TRUE);
}

void FrameBuffer::AttachTexture(i32 attachment, u32 texture, i32 level)
{
	textAttachments.push_back(texture);
	glNamedFramebufferTexture(id, attachment, texture, level);
}

void FrameBuffer::AttachRenderBuffer(i32 attachment, u32 renderbuffer)
{
	rboAttachments.push_back(renderbuffer);
	glNamedFramebufferRenderbuffer(id, attachment, GL_RENDERBUFFER, renderbuffer);
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
	i32 mask,
	i32 filter) const
{
	glBlitNamedFramebuffer(
		id, dest.id, 
		srcLowerX, srcLowerY, 
		srcUpperX, srcUpperY, 
		destLowerX, destLowerY, 
		destUpperX, destUpperY, 
		mask, 
		filter);
}

void FrameBuffer::SetWritingColorComponents(bool r, bool g, bool b, bool a) const
{
	glColorMaski(id, r, g, b, a);
}