#pragma once
#ifndef FONT_
#define FONT_

#include "config.h"
#include "VAO.h"

class Font
{
public:
	struct Character
	{
		unsigned int texture;
		glm::ivec2 size;
		glm::ivec2 bearing;
		unsigned int advance;
	};
	std::unordered_map<char, Character> characters;
	int height;

	Font(const std::string& font_filepath, int height);

	Character getCharacter(char c);
};

#endif