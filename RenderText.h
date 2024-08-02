#pragma once
#include "Config.h"
#include "Font.h"
#include "ObjectManager.h"

void renderText(
	const std::string& text,
	const std::string& font_nane,
	const std::string& shader_name,
	GLFWwindow* window,
	const glm::vec2& pos,
	float scale,
	const glm::vec3& color
);