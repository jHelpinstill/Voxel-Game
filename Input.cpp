#include "Input.h"

Input::Input(GLFWwindow* window)
{
	this->window = window;
	objects.push_back(this);

	glfwSetKeyCallback(window, keyCallback);

	for (Key& key : keys)
	{
		key.state = key.prev_state = GLFW_RELEASE;
		key.pressed = key.held = key.released = false;
	}
}

void Input::update()
{
	for (Key& key : keys)
	{
		key.held = key.state;
		key.pressed = (key.state == GLFW_PRESS & &key.prev_state == GLFW_RELEASE);
		key.released = (key.state == GLFW_RELEASE & &key.prev_state == GLFW_PRESS);
		key.prev_state = key.state;
	}

	double prevx, prevy;
	prevx = mouse.x;
	prevy = mouse.y;
	glfwGetCursorPos(window,& mouse.x,& mouse.y);
	mouse.delta.x = mouse.x - prevx;
	mouse.delta.y = mouse.y - prevy;
}

bool Input::keyPressed(int key)
{
	return keys[key].pressed;
}

bool Input::keyHeld(int key)
{
	return keys[key].held;
}

bool Input::keyReleased(int key)
{
	return keys[key].released;
}

void Input::lockCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void Input::freeCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//std::cout << key << std::endl;

	if (action == GLFW_REPEAT)
		return;

	for (Input* input : objects)
	{
		if (input->window == window)
		{
			input->keys[key].state = action;
			break;
		}
	}
}

std::vector<Input*> Input::objects;