#pragma once
#ifndef INPUT_
#define INPUT_

#include "config.h"

class Input
{
private:
	GLFWwindow* window;
	struct Key
	{
		int state, prev_state;
		bool pressed, held, released;
	};
	Key keys[GLFW_KEY_LAST];

	static std::vector<Input*> objects;

public:
	struct {
		double x, y;
		glm::vec2 delta;
		bool right_pressed;
		bool left_pressed;
	} mouse;

	Input(GLFWwindow* window);

	void update();

	bool keyPressed(int key);
	bool keyHeld(int key);
	bool keyReleased(int key);

	void lockCursor();
	void freeCursor();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif