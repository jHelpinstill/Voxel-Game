#pragma once
#ifndef TEXT_BOX_
#define TEXT_BOX_

#include "config.h"
#include "Font.h"
#include "Shader.h"
#include "Decal.h"
#include "RenderText.h"

class Textbox
{
public:
	Font* font;
	Shader* shader;
	std::string text;
	float scale;
	glm::vec3 color;

	Textbox(const std::string& text, Font* font, Shader* shader, float scale, const glm::vec3 color)
		: text(text), font(font), shader(shader), scale(scale), color(color) {}

	static void drawTextbox(Decal* decal, GLFWwindow* window, void* obj);
};

#endif