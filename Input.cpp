#include "Input.h"

Input::Input(GLFWwindow* window)
{
	this->window = window;

}

void Input::update()
{
	double prevx, prevy;
	prevx = mouse.x;
	prevy = mouse.y;
	glfwGetCursorPos(window, &mouse.x, &mouse.y);
	mouse.delta.x = mouse.x - prevx;
	mouse.delta.y = mouse.y - prevy;
}

void Input::lockCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void Input::unlockCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}