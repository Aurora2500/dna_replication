#include "msaa.hpp"

const int SAMPLES = 4;

MSAA::MSAA(int width, int height)
	: m_width(width)
	, m_height(height)
	, m_color_rbo(rendering::RenderBufferTarget::Color)
	, m_depth_rbo(rendering::RenderBufferTarget::Depth)
{
	m_color_rbo.store_multisampled(width, height, SAMPLES);
	m_depth_rbo.store_multisampled(width, height, SAMPLES);
	m_fbo.attatch(m_color_rbo);
	m_fbo.attatch(m_depth_rbo);

	m_fbo.check();
}

void MSAA::bind() {
	m_fbo.bind_and_clear();
}

void MSAA::blit() {
	glBlitNamedFramebuffer(
		m_fbo.id(),
		0,
		0,0, m_width, m_height,
		0,0, m_width, m_height,
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR
	);
}

MSAA_Guard::MSAA_Guard(MSAA& msaa)
	: m_msaa(&msaa)
{
	m_msaa->bind();
}

MSAA_Guard::~MSAA_Guard() {
	if (m_msaa == nullptr) return;
	m_msaa->blit();
}