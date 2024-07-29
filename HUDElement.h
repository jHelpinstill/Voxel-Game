#pragma once
#ifndef HUD_ELEMENT
#define HUD_ELEMENT

#include "config.h"
#include "Shader.h"
#include "VAO.h"
#include "Transform.h"

class HUDElement
{
public:
	VAO* vao = nullptr;

	Shader* shader = nullptr;
	unsigned int texture;
	Transform transform;
	void* parent_obj = nullptr;

	std::vector<glm::vec3> verts;
	glm::vec4 color{};

	void (*drawFunc)(HUDElement* hud_element, void* obj) = nullptr;

	HUDElement(unsigned int texture, void (*drawFunc)(HUDElement*, void*));

	void draw();
};

#endif