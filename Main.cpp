#include "config.h"
#include "util.h"
#include "TriangleMesh.h"
#include "Shader.h"
#include "Camera.h"

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

	Shader tri_shader("shaders/vertex.txt", "shaders/fragment.txt");

	float verts[9] = {
		1.0f, 0.0f, -5.0f,
		0.0f, 1.0f, -5.0f,
		0.0f, 0.0f, -5.0f
	};
	float colors[9] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	TriangleMesh* tri = new TriangleMesh(verts, colors);

	Camera camera(window);


	double prev_time = 0;
	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		double dt = time - prev_time;
		prev_time = time;

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		float speed = 45;
		float angle = speed* time;
		tri_shader.setFloat("angle", angle);

		camera.setRotation(sin(time), glm::vec3(0, 1, 0));

		tri_shader.use();
		unsigned int camera_mat_location = glGetUniformLocation(tri_shader.ID, "camera_mat");
		glUniformMatrix4fv(camera_mat_location, 1, GL_FALSE, glm::value_ptr(camera.getMat()));

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