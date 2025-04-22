#include "Framebuffer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

void Framebuffer::Create()
{
	glCreateFramebuffers(1, &id);
}

void Framebuffer::Delete()
{
	u32 size;
	if (textAttachments.size() > 0)
	{
		size = 0;
		Array<u32, MAX_NUM_TEXTURE_ATTACHMENTS> ids{};
		for (auto& texture : textAttachments)
			ids[size++] = texture.id;
		glDeleteTextures(size, ids.data());
		textAttachments.clear();
	}
	
	if (rboAttachments.size() > 0)
	{
		size = 0;
		Array<u32, MAX_NUM_RBO_ATTACHMENTS> ids{};
		for (auto& texture : textAttachments)
			ids[size++] = texture.id;
		glDeleteRenderbuffers(size, ids.data());
		rboAttachments.clear();
	}
	
	if (IsValid())
	{
		glDeleteFramebuffers(1, &id);
		id = 0;
	}
}

Texture2D Framebuffer::GetTextureAttachment(u32 index)
{
	if (index >= textAttachments.size() || index >= MAX_NUM_TEXTURE_ATTACHMENTS)
	{
		CONSOLE_WARN("Cannot get texture attachment. Index {} is out of bounds.", index);
		return Texture2D();
	}
	return textAttachments.at(index);
}

Renderbuffer Framebuffer::GetRenderbufferAttachment(u32 index)
{
	if (index >= rboAttachments.size() || index >= MAX_NUM_RBO_ATTACHMENTS)
	{
		CONSOLE_WARN("Cannot get renderbuffer attachment. Index {} is out of bounds.", index);
		return Renderbuffer();
	}
	return rboAttachments.at(index);
}

void Framebuffer::Bind(FramebufferTarget target) const
{
	glBindFramebuffer(static_cast<u32>(target), id);
}

void Framebuffer::Unbind(FramebufferTarget target) const
{
	glBindFramebuffer(static_cast<u32>(target), 0);
}

FramebufferStatus Framebuffer::CheckStatus() const
{
	return static_cast<FramebufferStatus>(glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER));
}

bool Framebuffer::IsValid() const
{
	return (id != 0) && (glIsFramebuffer(id) == GL_TRUE);
}

void Framebuffer::AttachTexture(FramebufferAttachment attachment, Texture2D texture, i32 level)
{
	if (textAttachments.size() >= MAX_NUM_TEXTURE_ATTACHMENTS)
	{
		CONSOLE_WARN("Cannot add more texture attachments. Maximum limit of {} reached.", MAX_NUM_TEXTURE_ATTACHMENTS);
		return;
	}

	textAttachments.push_back(texture);
	glNamedFramebufferTexture(id, static_cast<u32>(attachment), texture.id, level);
}

void Framebuffer::AttachRenderBuffer(FramebufferAttachment attachment, Renderbuffer renderbuffer)
{
	if (rboAttachments.size() >= MAX_NUM_RBO_ATTACHMENTS)
	{
		CONSOLE_WARN("Cannot add more renderbuffer attachments. Maximum limit of {} reached.", MAX_NUM_RBO_ATTACHMENTS);
		return;
	}

	rboAttachments.push_back(renderbuffer);
	glNamedFramebufferRenderbuffer(id, static_cast<u32>(attachment), GL_RENDERBUFFER, renderbuffer.id);
}

void Framebuffer::Blit(
	const Framebuffer& dest,
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

void Framebuffer::SetWritingColorComponents(bool r, bool g, bool b, bool a) const
{
	glColorMaski(id, r, g, b, a);
}