#pragma once
#ifndef DECAL_
#define DECAL_

#include "config.h"
#include "Shader.h"
#include "VAO.h"

class Decal
{
public:
	VAO* vao = nullptr;

	Shader* shader = nullptr;
	unsigned int texture;
	glm::vec2 size;
	glm::vec2 pos;
	GLFWwindow* window = nullptr;
	void* parent_obj = nullptr;

	void (*drawFunc)(Decal* decal, GLFWwindow* window, void* obj) = nullptr;

	Decal(unsigned int texture, const glm::vec2& size, const glm::vec2& pos);
	~Decal();

	void draw();
	
	static void drawRelative(Decal* decal, GLFWwindow* window, void* obj);
	static void drawAbsolute(Decal* decal, GLFWwindow* window, void* obj);
	static void drawCentered(Decal* decal, GLFWwindow* window, void* obj);
};

#endif