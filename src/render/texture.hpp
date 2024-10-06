#pragma once

#include <string>
#include <span>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

namespace rendering {

enum class TextureSampling
{
	Linear,
};

enum class TextureWrapping
{
	Clamp,
	Border,
	Wrap,
};

class Texture
{
private:
	unsigned int m_id;
	TextureSampling m_sampling;
	TextureWrapping m_wrapping;
	unsigned int m_width, m_height;

public:
	Texture(TextureSampling sampling = TextureSampling::Linear, TextureWrapping wrapping = TextureWrapping::Border, glm::vec3 &&col = glm::vec3(0.0f, 0.0f, 0.0f));
	~Texture();

	void bind(int slot = 0) const;
	void unbind() const;

	void store(
			unsigned int width,
			unsigned int height);

	void load(
			unsigned int width,
			unsigned int height,
			unsigned char *data);

	inline unsigned int id() const { return m_id; }

};


}