#include "text.hpp"

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

TextLibrary::TextLibrary()
{
	FT_Error err = FT_Init_FreeType(&m_library);
	if (err)
	{
		std::cerr << "Error initializing FreeType library." << std::endl;
	}
}


TextLibrary::~TextLibrary()
{
	FT_Error err = FT_Done_FreeType(m_library);
	if (err)
	{
		std::cerr << "Error deinitializing FreeType library." << std::endl;
	}
}

FontFace TextLibrary::load_font(const std::string &path)
{
	FontFace face;
	std::cerr << "Loading font face: " << path << std::endl;
	FT_Error err = FT_New_Face(m_library, path.c_str(), 0, &face.m_face);
	if (err)
	{
		std::cerr << "Error loading font face." << std::endl;
	}

	err = FT_Set_Pixel_Sizes(face.m_face, 0, 48);
	if (err)
	{
		std::cerr << "Error setting font size." << std::endl;
		std::cerr << "Error code: " << err << std::endl;
	}
	return face;
}

FontFace::~FontFace()
{
	if (!m_face)
		return;
	FT_Error err = FT_Done_Face(m_face);
	std::cerr << "Deinitializing font face." << std::endl;
	if (err)
	{
		std::cerr << "Error deinitializing FreeType face." << std::endl;
	}
}

FontFace& FontFace::operator=(FontFace &&other)
{
	FT_Done_Face(m_face);
	m_face = other.m_face;
	other.m_face = nullptr;
	return *this;
}

FontFace::FontFace(FontFace &&other)
{
	m_face = other.m_face;
	other.m_face = nullptr;
}

BitMap FontFace::load_char(char c)
{
	BitMap bitmap;
	FT_Int32 flags = FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_SDF);
	FT_Error err = FT_Load_Char(m_face, c, flags);
	if (err)
	{
		std::cerr << "Error loading character." << std::endl;
	}
	bitmap.buffer = m_face->glyph->bitmap.buffer;
	bitmap.advance = m_face->glyph->advance.x >> 6;
	bitmap.size = {m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows};
	bitmap.bearing = {m_face->glyph->bitmap_left, m_face->glyph->bitmap_top};
	return bitmap;
}