#include "Config.h"

int main()
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		std::cout << "GLFW couldn't start" << std::endl;
		return -1;
	}

	window = glfwCreateWindow(640, 480, "Voxel Game", NULL, NULL);
	glfwMakeContextCurrent(window);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}