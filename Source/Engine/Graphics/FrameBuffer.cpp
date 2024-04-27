#include "FrameBuffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include "Core/Log/Logger.hpp"


FrameBuffer::FrameBuffer()
	: id{ static_cast<uint32_t>(-1) },
		_textAttachments{},
		_rboAttachments{}
{}

void FrameBuffer::Create()
{
	glCreateFramebuffers(1, &id);
}

void FrameBuffer::Delete()
{
	glDeleteFramebuffers(1, &id);

	for (Texture2D& texture : _textAttachments)
		texture.Delete();
	for (RenderBuffer& rbo : _rboAttachments)
		rbo.Delete();
	
	_textAttachments.clear();
	_rboAttachments.clear();

	id = static_cast<uint32_t>(-1);
}

void FrameBuffer::Bind(int target) const
{
	glBindFramebuffer(target, id);
}

void FrameBuffer::Unbind(int target) const
{
	glBindFramebuffer(target, 0);
}

int FrameBuffer::CheckStatus() const
{
	return glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
}

void FrameBuffer::AttachTexture(int attachment, const Texture2D& texture, int level)
{
	glNamedFramebufferTexture(id, attachment, texture.id, level);

	_textAttachments.push_back(texture);
}

void FrameBuffer::AttachRenderBuffer(int attachment, const RenderBuffer& renderbuffer)
{
	glNamedFramebufferRenderbuffer(id, attachment, GL_RENDERBUFFER, renderbuffer.id);
	
	_rboAttachments.push_back(renderbuffer);
}

void FrameBuffer::Blit(
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
	int filter) const
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


