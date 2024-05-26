#include "config.h"
#include "util.h"
#include "TriangleMesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

int main()
{
	if (!glfwInit())	// MUST RUN BEFORE GLAD INIT
	{
		std::cout << "GLFW couldn't start" << std::endl;
		return -1;
	}
	//GLFWwindow* window;
#ifndef LOCK_FRAMERATE
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
#endif
	GLFWwindow* window = glfwCreateWindow(640, 480, "Voxel Game", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	// MUST RUN BEFORE ANY OTHER OPENGL FUNCTION
	{
		glfwTerminate();
		std::cout << "Glad couldn't start" << std::endl;
		return -1;
	}
	//printFileToTerminal("shaders/fragment.txt", true);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

	Shader mesh_shader("shaders/MeshVertex.txt", "shaders/MeshFragment.txt");
	Camera camera(window);

	Mesh* box_mesh = Mesh::makeBox(1, 1, 0.5, glm::vec3(-0.5, -0.75, -2.0));
	box_mesh->attachCamera(camera);
	box_mesh->attachShader(mesh_shader);


	double prev_time = 0;
	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		double dt = time - prev_time;
		prev_time = time;

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		camera.setRotation(0.5*sin(time), glm::vec3(0, 1, 0));
		box_mesh->draw();

#ifdef LOCK_FRAMERATE
		glfwSwapBuffers(window);
#else
		glFlush();
#endif
	}

	glfwTerminate();
	return 0;
}