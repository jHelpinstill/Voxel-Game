#pragma once
#ifndef HUD_ELEMENT
#define HUD_ELEMENT

#include "config.h"
#include "Shader.h"
#include "Transform.h"

class HUDElement
{
public:
	unsigned int VAO;
	unsigned int VBO;

	Shader* shader;

	std::vector<glm::vec3> verts;
	glm::vec4 color;

	HUDElement() {}

	void draw();
	void createVAO();

	void addRect(float x, float y, float w, float h);
};

#endif