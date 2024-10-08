#include "render_buffer.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

namespace rendering
{

RenderBuffer::RenderBuffer(RenderBufferTarget target)
	: m_target(target)
{
	glCreateRenderbuffers(1, &m_id);
}

RenderBuffer::~RenderBuffer()
{
	glDeleteRenderbuffers(1, &m_id);
}

void RenderBuffer::store(
		unsigned int width,
		unsigned int height)
{
	if (m_width == width && m_height == height) return;
	glNamedRenderbufferStorage(
			m_id,
			(m_target==RenderBufferTarget::Color? GL_RGBA8 : GL_DEPTH24_STENCIL8),
			width,
			height);
	m_width = width;
	m_height = height;
}

void RenderBuffer::store_multisampled(
		unsigned int width,
		unsigned int height,
		unsigned int samples)
{
	m_width = width;
	m_height = height;
	glBindRenderbuffer(GL_RENDERBUFFER, m_id);
	glRenderbufferStorageMultisample(
		GL_RENDERBUFFER, samples, (m_target == RenderBufferTarget::Color? GL_RGB8 : GL_DEPTH24_STENCIL8), width, height
	);
}

}