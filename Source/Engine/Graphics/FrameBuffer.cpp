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
	uint64_t numTextures = _textAttachmentIDs.size();
	if (numTextures > 0)
		glDeleteTextures(numTextures, _textAttachmentIDs.data());
	
	uint64_t numRBOs = _rboAttachmentIDs.size();
	if (numRBOs > 0)
		glDeleteRenderbuffers(numRBOs, _rboAttachmentIDs.data());

	vector<uint32_t>().swap(_textAttachmentIDs);
	vector<uint32_t>().swap(_rboAttachmentIDs);

	glDeleteFramebuffers(1, &id);
	id = 0;
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

void FrameBuffer::AttachTexture(int attachment, uint32_t textureID, int level)
{
	glNamedFramebufferTexture(id, attachment, textureID, level);
	_textAttachmentIDs.push_back(textureID);
}

void FrameBuffer::AttachRenderBuffer(int attachment, uint32_t renderbufferID)
{
	glNamedFramebufferRenderbuffer(id, attachment, GL_RENDERBUFFER, renderbufferID);
	_rboAttachmentIDs.push_back(renderbufferID);
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

void FrameBuffer::SetWritingColorComponents(bool r, bool g, bool b, bool a) const
{
	glColorMaski(id, r, g, b, a);
}