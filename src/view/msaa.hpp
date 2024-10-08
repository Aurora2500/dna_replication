#pragma once

#include "render/frame_buffer.hpp"

class MSAA {
private:
	int m_width;
	int m_height;

	rendering::FrameBuffer m_fbo;
	rendering::RenderBuffer m_color_rbo;
	rendering::RenderBuffer m_depth_rbo;
public:
	MSAA(int width, int height);

	void bind();

	void blit();
};

class MSAA_Guard
{
private:
	MSAA* m_msaa;
public:
	MSAA_Guard(MSAA& msaa);
	~MSAA_Guard();
};