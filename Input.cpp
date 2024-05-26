#include "Input.h"

Input::Input(GLFWwindow* window)
{
	glfwSetKeyCallback(window, keyCallback);
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}