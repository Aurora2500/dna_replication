#include "text.hpp"

#include <cmath>
#include <cstring>
#include <algorithm>

#include <GL/glew.h>
#include <GL/gl.h>

#include "mesh.hpp"
#include "assets/text.hpp"

constexpr int NUM_COLS = 16;

namespace rendering
{

TextAtlas::TextAtlas()
	: m_texture(TextureSampling::Linear, TextureWrapping::Clamp)
{
}

void TextAtlas::load(FontFace& font)
{
	m_width = 0;
	m_height = 0;

	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int y_down = 0;
	unsigned int col = 0;
	

	for (int c = 33; c < 127; c++)
	{
		auto bitmap = font.load_char(c);
		m_glyphs.insert({
			static_cast<char>(c),
			{
				.pos = {x, y},
				.size = bitmap.size,
				.bearing = bitmap.bearing,
				.advance = bitmap.advance
			}
		});
		x += bitmap.size.x;
		y_down = std::max(y_down, y + bitmap.size.y);

		m_width = std::max(m_width, x);
		m_height = std::max(m_height, y_down);

		col++;
		if (col == NUM_COLS)
		{
			col = 0;
			x = 0;
			y = y_down + 1;
		}
	}

	std::vector<unsigned char> atlas_buffer(m_width * m_height, 0);

	for (int c = 33; c < 127; c++)
	{
		auto bitmap = font.load_char(c);
		auto g = m_glyphs[c];
		for (unsigned int i = 0; i < bitmap.size.y; i++)
		{
			std::memcpy(
				atlas_buffer.data() + (g.pos.y + bitmap.size.y - i - 1) * (m_width + 3) + g.pos.x,
				bitmap.buffer + i * bitmap.size.x,
				bitmap.size.x
			);
		}
	}

	m_texture.load(m_width, m_height, atlas_buffer.data());
}

Mesh2D TextAtlas::create_text(const std::string &text, float scale) {
	Mesh2D mesh;

	int i = 0;
	float x = 0;

	for (auto c: text)
	{
		if (c == ' ') {
			x += scale;
			continue;
		}
		auto g = m_glyphs[c];
		float xpos = x + g.bearing.x * scale;
		float ypos = (g.bearing.y - g.size.y) * scale;

		glm::vec2 screen_pos = {xpos, ypos};

		float w = g.size.x * scale;
		float h = g.size.y * scale;

		glm::vec2 atlas_size = {m_width, m_height};

		mesh.vertices.push_back({
			screen_pos,
			glm::vec2(g.pos) / atlas_size,
		});
		mesh.vertices.push_back({
			screen_pos + glm::vec2(w, 0),
			glm::vec2(g.pos.x + g.size.x, g.pos.y) / atlas_size,
		});
		mesh.vertices.push_back({
			screen_pos + glm::vec2(0, h),
			glm::vec2(g.pos.x, g.pos.y + g.size.y) / atlas_size,
		});
		mesh.vertices.push_back({
			screen_pos + glm::vec2(w, h),
			glm::vec2(g.pos + g.size) / atlas_size,
		});

		mesh.indices.push_back(i + 0);
		mesh.indices.push_back(i + 1);
		mesh.indices.push_back(i + 2);
		mesh.indices.push_back(i + 1);
		mesh.indices.push_back(i + 3);
		mesh.indices.push_back(i + 2);
		i += 4;

		x += g.advance * scale;
	}

	mesh.make_buffers();
	return mesh;
}

}