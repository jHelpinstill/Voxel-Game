#include "config.h"
#include "util.h"
#include "TriangleMesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Input.h"
#include "CameraController.h"

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
	glEnable(GL_CULL_FACE);
	Input input(window);
	//printFileToTerminal("shaders/fragment.txt", true);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

	Shader mesh_shader("shaders/MeshVertex.txt", "shaders/MeshFragment.txt");
	Camera camera(window);
	//camera.setPos(glm::vec3(0, 1, 0));

	CameraController player(camera, input);
	player.constrainLook(glm::vec3(0, 1, 0));

	Mesh* box_mesh = Mesh::makeBox(1, 1, 0.5, glm::vec3(0, -0.75, -2.0));
	box_mesh->attachCamera(camera);
	box_mesh->attachShader(mesh_shader);


	double prev_time = 0;
	bool paused = true;
	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		double dt = time - prev_time;
		prev_time = time;

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
		box_mesh->draw();

		glm::vec3 c_pos = camera.getPos();
		if (!paused)
		{
			std::cout << "camera pos:\t" << camera.pos.x << "\t" << camera.pos.y << "\t" << camera.pos.z << std::endl;
			//std::cout << "camera col4:\t" << c_pos.x << "\t" << c_pos.y << "\t" << c_pos.z << std::endl;
		}
#ifdef LOCK_FRAMERATE
		glfwSwapBuffers(window);
#else
		glFlush();
#endif
	}

	glfwTerminate();
	return 0;
}