#include "ShadowMap.hpp"

#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

ShadowMap::ShadowMap(int width, int height)
{
	/* Create a framebuffer object for rendering the depth map */
	glGenFramebuffers(1, &_fbo);

	/* 
		Create a 2D texture that we'll use as the framebuffer's depth buffer. 
		We also give the texture a width and height: this is the resolution of the depth map 
	*/
	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/* Resolve the problem of over sampling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, Array<float, 4>{ 1.0f, 1.0f, 1.0f, 1.0f }.data());


	/* With the generated depth texture we can attach it as the framebuffer's depth buffer */
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	assert(CheckStatus() && "ShadowMap framebuffer is not complete!");
}
ShadowMap::~ShadowMap()
{
	glDeleteFramebuffers(1, &_fbo);

	glDeleteTextures(1, &_depthMap);
}

void ShadowMap::Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, _fbo); }
void ShadowMap::Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

bool ShadowMap::CheckStatus() const { return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE; }