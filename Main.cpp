#include "config.h"
#include "util.h"
#include "TriangleMesh.h"
#include "Shader.h"

int main()
{
	if (!glfwInit())	// MUST RUN BEFORE GLAD INIT
	{
		std::cout << "GLFW couldn't start" << std::endl;
		return -1;
	}
	GLFWwindow* window;
#ifndef LOCK_FRAMERATE
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
#endif
	window = glfwCreateWindow(640, 480, "Voxel Game", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	// MUST RUN BEFORE ANY OTHER OPENGL FUNCTION
	{
		glfwTerminate();
		std::cout << "Glad couldn't start" << std::endl;
		return -1;
	}
	//printFileToTerminal("shaders/fragment.txt", true);

	float r = 1.0f;
	float verts[9] = {
		0.0f, r, 0.0f,
		-r * sqrt(3) * 0.5, -0.5 * r, 0.0f,
		r * sqrt(3) * 0.5, -0.5 * r, 0.0f
	};
	float colors[9] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	TriangleMesh* tri = new TriangleMesh(verts, colors);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

	//unsigned int shader = makeShader(
	//	"shaders/vertex.txt",
	//	"shaders/fragment.txt"
	//);

	Shader tri_shader("shaders/vertex.txt", "shaders/fragment.txt");

	double prev_time = 0;
	int c = 0;
	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		double dt = time - prev_time;
		prev_time = time;

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		tri_shader.use();

		float speed = 3.14159;
		float angle = speed * time;
		tri_shader.setFloat("angle", angle);

		tri->draw();

#ifdef LOCK_FRAMERATE
		glfwSwapBuffers(window);
#else
		glFlush();
#endif
	}

	glfwTerminate();
	return 0;
}