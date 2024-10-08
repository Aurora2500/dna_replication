#pragma once

namespace rendering
{

enum class RenderBufferTarget {
	Color,
	Depth,
};

class RenderBuffer
{
private:
	unsigned int m_id;
	unsigned int m_width, m_height;
	RenderBufferTarget m_target;

public:
	RenderBuffer(RenderBufferTarget target);
	~RenderBuffer();

	void store(
			unsigned int width,
			unsigned int height);

	void store_multisampled(
		unsigned int width,
		unsigned int height,
		unsigned int samples
	);

	inline unsigned int id() const { return m_id; }
	inline RenderBufferTarget target() const { return m_target; }
};

}