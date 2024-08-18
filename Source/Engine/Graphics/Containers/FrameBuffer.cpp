#include "FrameBuffer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

FrameBuffer::FrameBuffer()
	: id{ 0 },
		_textAttachmentIDs{},
		_rboAttachmentIDs{}
{}

void FrameBuffer::Create()
{
	glCreateFramebuffers(1, &id);
}

void FrameBuffer::Delete()
{
	u64 numTextures = _textAttachmentIDs.size();
	if (numTextures > 0)
		glDeleteTextures(numTextures, _textAttachmentIDs.data());
	
	u64 numRBOs = _rboAttachmentIDs.size();
	if (numRBOs > 0)
		glDeleteRenderbuffers(numRBOs, _rboAttachmentIDs.data());

	Vector<u32>().swap(_textAttachmentIDs);
	Vector<u32>().swap(_rboAttachmentIDs);

	glDeleteFramebuffers(1, &id);
	id = 0;
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

void FrameBuffer::AttachTexture(i32 attachment, u32 textureID, i32 level)
{
	glNamedFramebufferTexture(id, attachment, textureID, level);
	_textAttachmentIDs.push_back(textureID);
}

void FrameBuffer::AttachRenderBuffer(i32 attachment, u32 renderbufferID)
{
	glNamedFramebufferRenderbuffer(id, attachment, GL_RENDERBUFFER, renderbufferID);
	_rboAttachmentIDs.push_back(renderbufferID);
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