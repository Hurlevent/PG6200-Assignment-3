#include "ShadowFBO.h"
#include "GLUtils/GLUtils.hpp"


ShadowFBO::ShadowFBO(unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;
	
	//throw std::runtime_error("ShadowFBO not implemented yet...");

	// Initialize Depth Texture

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	// Create FBO and attach buffers

	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glDrawBuffer(GL_NONE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Check for completeness
	CHECK_GL_ERRORS();
	CHECK_GL_FBO_COMPLETENESS();
}

ShadowFBO::~ShadowFBO() {
	glDeleteFramebuffersEXT(1, &fbo);
}

void ShadowFBO::bind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
}

void ShadowFBO::unbind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}