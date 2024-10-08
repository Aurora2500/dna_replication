#include "frame_buffer.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

namespace rendering
{

FrameBuffer::FrameBuffer()
{
	glCreateFramebuffers(1, &m_id);
}

void FrameBuffer::check() {
	auto status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		std::cout << "Complete frame buffer" << std::endl;
		break;
	default:
		std::cout << "Incomplete frame buffer" << std::endl;
		break;
	}
}

void FrameBuffer::attatch(const Texture& texture)
{
	glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0, texture.id(), 0);
}

void FrameBuffer::attatch(const RenderBuffer& render_buffer)
{
	glNamedFramebufferRenderbuffer(
			m_id,
			render_buffer.target() == RenderBufferTarget::Color? GL_COLOR_ATTACHMENT0 : GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER,
			render_buffer.id());
}

void FrameBuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void FrameBuffer::bind_and_clear(GLbitfield bits) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	float r = 170.0f / 255.0;
	float g = 160.0f / 255.0;
	float b = 200.0f / 255.0;
	glClearColor(r, g, b, 1.0f);
	glClear(bits);
}

}