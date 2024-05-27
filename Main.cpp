#include "config.h"
#include "util.h"
#include "TriangleMesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Input.h"
#include "CameraController.h"

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
	//GLFWwindow* window;
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
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	Input input(window);
	//printFileToTerminal("shaders/fragment.txt", true);

	glClearColor(0.25f, 0.85f, 1.0f, 1.0f);

	Shader mesh_shader("shaders/MeshVertex.txt", "shaders/MeshFragment.txt");
	Camera camera(window);
	camera.transform.pos = glm::vec3(0, 1, 3);

	CameraController player(camera, input);
	player.constrainLook(glm::vec3(0, 1, 0));
	player.move_speed = 4;

	Mesh* box_mesh = Mesh::makeBox(1, 1, 0.5, glm::vec3(0, 1, 0));// , glm::vec3(0, -0.75, -2.0));
	box_mesh->attachCamera(camera);
	box_mesh->attachShader(mesh_shader);

	Mesh* floor = Mesh::makeBox(10, 10, 0.1, glm::vec3(0, -0.05, 0));// , glm::vec3(0, -0.1, 0));
	floor->color = glm::vec3(0.0, 0.7, 0.0);
	floor->use_color = true;
	floor->updateVAO();
	floor->attachCamera(camera);
	floor->attachShader(mesh_shader);


	double prev_time = 0;
	bool paused = true;
	float box_speed = 120;
	while (!glfwWindowShouldClose(window))
	{
		if (window_resized)
		{
			window_resized = false;
			camera.findAspectRatio(window);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double time = glfwGetTime();
		double dt = time - prev_time;
		prev_time = time;

		box_mesh->transform.rotate(glm::radians(box_speed) * dt, glm::vec3(0, 1, 0));
		box_mesh->transform.translate(glm::vec3(0, cos(time * 3) * 1 * dt, 0));

		glfwPollEvents();
		input.update();
		if (input.keyPressed(GLFW_KEY_ESCAPE) && paused)
		{
			paused = false;
			input.lockCursor();
			std::cout << "UNPAUSED" << std::endl;
		}
		else if (input.keyPressed(GLFW_KEY_ESCAPE) && !paused)
		{
			paused = true;
			input.freeCursor();
			std::cout << "PAUSED" << std::endl;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		if(!paused)
			player.update(dt);
		floor->draw();
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