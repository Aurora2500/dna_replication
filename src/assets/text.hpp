#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

#include <glm/glm.hpp>

class FontFace;

class TextLibrary
{
private:
	FT_Library m_library;

public:
	TextLibrary();
	~TextLibrary();

	FontFace load_font(const std::string &path);
};

struct BitMap
{
	unsigned char *buffer;
	glm::uvec2 size;
	glm::ivec2 bearing;
	int advance;
};

class FontFace
{
private:
	FT_Face m_face;
	FontFace() = default;

public:
	~FontFace();

	FontFace& operator=(FontFace &&other);
	FontFace(FontFace &&other);

	BitMap load_char(char c);

	friend class TextLibrary;
};