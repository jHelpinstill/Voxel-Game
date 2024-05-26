#pragma once
#ifndef INPUT_
#define INPUT_

#include "config.h"

class Input
{
private:
	GLFWwindow* window;
public:
	struct {
		double x, y;
		glm::vec2 delta;
		bool right_pressed;
		bool left_pressed;
	} mouse;

	Input(GLFWwindow* window);

	void update();

	void lockCursor();
	void unlockCursor();
};

#endif