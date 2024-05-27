#include "Game.h"

enum
{
	PAUSED,
	RUNNING
} state;

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

	Shader* mesh_shader = new Shader("mesh_shader", "shaders/MeshVertex.txt", "shaders/MeshFragment.txt");
	shaders.push_back(mesh_shader);

	Mesh* box_mesh = Mesh::makeBox("box_mesh", 1, 1, 0.5, glm::vec3(0, 1, 0));
	box_mesh->attachCamera(*camera);
	box_mesh->attachShader(*mesh_shader);
	meshes.push_back(box_mesh);

	Mesh* floor = Mesh::makeBox("floor", 10, 10, 0.1, glm::vec3(0, -0.05, 0));
	floor->color = glm::vec3(0.0, 0.7, 0.0);
	floor->use_color = false;
	floor->updateVAO();
	floor->attachCamera(*camera);
	floor->attachShader(*mesh_shader);	
	meshes.push_back(floor);
}

Shader* Game::getShaderByName(const std::string& name)
{
	for (Shader* shader : shaders)
	{
		if (shader->name == name)
		{
			return shader;
		}
	}
	return nullptr;
}

Mesh* Game::getMeshByName(const std::string& name)
{
	for (Mesh* mesh : meshes)
	{
		if (mesh->name == name)
		{
			return mesh;
		}
	}
	return nullptr;
}

void Game::stateMachine(double dt, unsigned int texture)
{
	input->update();
	double time = glfwGetTime();

	switch (state)
	{
	case PAUSED:
	{
		if (input->keyPressed(GLFW_KEY_ESCAPE))
		{
			state = RUNNING;
			input->lockCursor();
			std::cout << "RUNNING" << std::endl;
		}

		drawMeshes(texture);
		break;
	}
	case RUNNING:
	{
		if (input->keyPressed(GLFW_KEY_ESCAPE))
		{
			state = PAUSED;
			input->freeCursor();
			std::cout << "PAUSED" << std::endl;
		}

		player->update(dt);
		std::cout << 1.0 / dt << std::endl;

		float box_speed = 120;
		Mesh* box_mesh = getMeshByName("box_mesh");
		if (box_mesh != nullptr)
		{
			box_mesh->transform.rotate(glm::radians(box_speed) * dt, glm::vec3(0, 1, 0));
			box_mesh->transform.translate(glm::vec3(0, cos(time * 3) * 1 * dt, 0));
		}

		drawMeshes(texture);
		break;
	}
	}
}

void Game::drawMeshes(unsigned int texture)
{
	for (Mesh* mesh : meshes)
	{
		mesh->draw(texture);
	}
}