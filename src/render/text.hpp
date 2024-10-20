#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "texture.hpp"
#include "assets/text.hpp"
#include "mesh.hpp"

namespace rendering
{

struct Glyph
{
	glm::ivec2 pos;
	glm::ivec2 size;
	glm::ivec2 bearing;
	int advance;
};


struct FaceData
{
	std::vector<unsigned char> data;
	unsigned int width, height, channels;
};

class TextAtlas
{
private:
	Texture m_texture;
	unsigned int m_width, m_height;
	std::unordered_map<char, Glyph> m_glyphs;

public:
	TextAtlas();

	void load(FontFace& font);

	inline Texture& texture() { return m_texture; }

	Mesh2D create_text(std::string const&text, float scale);
};

}