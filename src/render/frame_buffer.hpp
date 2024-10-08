#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

#include "texture.hpp"
#include "render_buffer.hpp"

namespace rendering
{

class FrameBuffer
{
private:
	unsigned int m_id;

public:
	FrameBuffer();

	void check();

	void attatch(const Texture& texture);
	void attatch(const RenderBuffer& render_buffer);

	void bind() const;
	void bind_and_clear(GLbitfield bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) const;
	inline static void bind_default() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	inline unsigned int id() const { return m_id; }
};

}