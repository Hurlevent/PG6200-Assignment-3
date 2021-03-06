#include "ShadowFBO.h"

#include "GLUtils/GLUtils.h"

ShadowFBO::ShadowFBO(unsigned int width, unsigned int height) {
	this->m_width = width;
	this->m_height = height;

	// Initialize Depth Texture

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	// Create FBO and attach buffers

	glGenFramebuffersEXT(1, &m_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
	glDrawBuffer(GL_NONE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, m_texture, 0);

	glClear(GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Check for completeness
	CHECK_GL_ERRORS();
	CHECK_GL_FBO_COMPLETENESS();
}

ShadowFBO::~ShadowFBO() {
	glDeleteFramebuffersEXT(1, &m_fbo);
}

void ShadowFBO::bind() const {
	glBindFramebufferEXT(GL_FRAMEBUFFER, m_fbo);
}

void ShadowFBO::unbind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}
