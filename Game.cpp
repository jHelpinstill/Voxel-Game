#include "Game.h"

Game::Game(GLFWwindow* window)
{
	this->window = window;
	state = PAUSED;
	setup();
}

Game::~Game()
{
	delete input;
	delete camera;
}

void Game::setup()
{
	input = new Input(window);

	camera = new Camera(window);
	camera->transform.pos = glm::vec3(0, 1, 3);

	player = new CameraController(*camera, *input);
	player->constrainLook(glm::vec3(0, 1, 0));
	player->move_speed = 4;

	createShader("texture_shader", "shaders/meshVertex.txt", "shaders/meshFragment.txt");
	createShader("color_shader", "shaders/meshColorVertex.txt", "shaders/meshColorFragment.txt");

	createTexture("smiley", "textures/smiley.png", true);
	createTexture("crate", "textures/crate.jpg");

	createTexturedBox("box_origin", glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), "smiley");
	createTexturedBox("crate", glm::vec3(1, 1, 1), glm::vec3(-2, 0, -2), "crate", "meshes/box_two_face_UV.txt");
	createPlane("ground", glm::vec2(10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 0.7, 0));
}

struct
{
	double history[1000]{};
	int index = 0;
	double sum = 0;
	double avg() { return sum / 1000; }
	void update(double fps)
	{
		sum += fps;
		sum -= history[index];
		history[index] = fps;
		++index %= 1000;
	}
} avg_fps;

void Game::stateMachine(double dt)
{
	input->update();
	double time = glfwGetTime();

	switch (state)
	{
	case PAUSED:
	{
		if (input->keyPressed(GLFW_KEY_ESCAPE) || input->mouse.left.pressed)
		{
			state = RUNNING;
			input->lockCursor();
			std::cout << "RUNNING" << std::endl;
		}

		drawMeshes();
		break;
	}
	case RUNNING:
	{
		if (input->keyPressed(GLFW_KEY_ESCAPE))
		{
			state = PAUSED;
			input->freeCursor();
			std::cout << "PAUSED" << std::endl;
			std::cout << "avg fps: " << avg_fps.avg() << std::endl;
		}

		player->update(dt);
		avg_fps.update(1.0 / dt);

		if (input->mouse.left.pressed)
		{
			removeMesh("crate");
		}
		//std::cout << 1.0 / dt << " avg: " << avg_fps.avg() << std::endl;

		drawMeshes();
		break;
	}
	}
}

void Game::drawMeshes()
{
	for (auto mesh : meshes)
		mesh.second->draw(camera);
}