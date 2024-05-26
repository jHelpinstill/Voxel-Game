#pragma once
#ifndef INPUT_
#define INPUT_

#include "config.h"

class Input
{
public:
	Input(GLFWwindow* window);

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif