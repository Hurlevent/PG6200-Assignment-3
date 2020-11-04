#ifndef _SHADOWFBO_HPP__
#define _SHADOWFBO_HPP__

#include <GL/glew.h>

class ShadowFBO {
public:
	ShadowFBO(unsigned int width, unsigned int height);
	~ShadowFBO();

	void bind() const;
	static void unbind();

	unsigned int getWidth() const {return m_width; }
	unsigned int getHeight() const {return m_height; }

	GLuint getTexture() const { return m_texture; }

private:
	GLuint m_fbo;
	GLuint m_depth;
	GLuint m_texture;
	unsigned int m_width, m_height;
};

#endif
