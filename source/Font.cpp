#include "Font.h"

Font::Font(const std::string& font_filepath, int height) : height(height)
{
	FT_Library ft_lib;
	if (FT_Init_FreeType(&ft_lib))
	{
		std::cout << "ERROR: Could note initilize FreeType library" << std::endl;
		while (1)
			;
	}

	FT_Face face;
	if (FT_New_Face(ft_lib, font_filepath.c_str(), 0, &face))
	{
		std::cout << "ERROR: failed to load font" << std::endl;
		while (1)
			;
	}

	FT_Set_Pixel_Sizes(face, 0, height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character ch = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};

		characters[c] = ch;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft_lib);
}

Font::Character Font::getCharacter(char c)
{
	try
	{
		return characters.at(c);
	}
	catch (std::out_of_range)
	{
		return Character{};
	}
}