#include "config.h"
#include "util.h"
#include "Game.h"

bool window_resized = false;

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);
	window_resized = true;
}

int main()
{
	if (!glfwInit())	// MUST RUN BEFORE GLAD INIT
	{
		std::cout << "GLFW couldn't start" << std::endl;
		return -1;
	}

#ifndef LOCK_FRAMERATE
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
#endif
	GLFWwindow* window = glfwCreateWindow(640, 480, "Voxel Game", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	// MUST RUN BEFORE ANY OTHER OPENGL FUNCTION
	{
		glfwTerminate();
		std::cout << "Glad couldn't start" << std::endl;
		return -1;
	}

	glClearColor(0.25f, 0.85f, 1.0f, 1.0f);

	Game game(window);

	//unsigned int texture = createTexture("textures/smiley.png");
	float frame_rate = 0.0;
	while (!glfwWindowShouldClose(window))
	{
		if (window_resized)
		{
			window_resized = false;
			game.camera->findAspectRatio(window);//camera.findAspectRatio(window);
		}
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		game.stateMachine(getDeltaTime(frame_rate));

#ifdef LOCK_FRAMERATE
		glfwSwapBuffers(window);
#else
		glFlush();
#endif
	}

	glfwTerminate();
	return 0;
}