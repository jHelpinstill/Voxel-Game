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
	} keys[GLFW_KEY_LAST + 1];

	static std::vector<Input*> objects;

public:
	struct Mouse
	{
		double x, y;
		glm::vec2 delta;
		struct Button
		{
			int state, prev_state;
			bool pressed, held, released;
		};
		union
		{
			Button buttons[GLFW_MOUSE_BUTTON_LAST + 1];
			Button left, right;
		};

	} mouse;

	Input(GLFWwindow* window);

	void update();

	bool keyPressed(int key);
	bool keyHeld(int key);
	bool keyReleased(int key);

	void lockCursor();
	void freeCursor();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};

#endif