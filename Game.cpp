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

	shaders.push_back(new Shader("texture_shader", "shaders/meshVertex.txt", "shaders/meshFragment.txt"));
	shaders.push_back(new Shader("color_shader", "shaders/meshColorVertex.txt", "shaders/meshColorFragment.txt"));

	textures.push_back(new Texture("smiley", createTexture("textures/smiley.png", true)));
	textures.push_back(new Texture("crate", createTexture("textures/crate.jpg")));

	createTexturedBox("box_origin", glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), "smiley");
	createTexturedBox("crate", glm::vec3(1, 1, 1), glm::vec3(-2, 0, -2), "crate", "meshes/box_two_face_UV.txt");

	Mesh* floor = Mesh::makePlane("floor", 10, 10);
	floor->color = glm::vec3(0.0, 0.7, 0.0);
	floor->useColorMode();
	floor->attachShader(getShaderByName("color_shader"));
	
	meshes.push_back(floor);
}

Shader* Game::getShaderByName(const std::string& name)
{
	for (Shader* shader : shaders)
		if (shader->name == name)
			return shader;

	std::cout << "couldn't find shader \"" << name << "\"" << std::endl;
	return nullptr;
}

Mesh* Game::getMeshByName(const std::string& name)
{
	for (Mesh* mesh : meshes)
		if (mesh->name == name)
			return mesh;

	std::cout << "couldn't find mesh \"" << name << "\"" << std::endl;
	return nullptr;
}

unsigned int Game::getTextureByName(const std::string& name)
{
	for (Texture* tex : textures)
		if (tex->name == name)
			return tex->ID;
	
	std::cout << "couldn't find texture \"" << name << "\"" << std::endl;
	return 0;
}

struct
{
	double history[1000];
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
		if (input->keyPressed(GLFW_KEY_ESCAPE))
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
			std::cout << 1.0 / dt << " avg: " << avg_fps.avg() << std::endl;
			for (int i = 0; i < 1000; i++)
				std::cout << avg_fps.history[i] << std::endl;
		}

		player->update(dt);
		avg_fps.update(1.0 / dt);
		//std::cout << 1.0 / dt << " avg: " << avg_fps.avg() << std::endl;

		drawMeshes();
		break;
	}
	}
}

void Game::drawMeshes()
{
	for (Mesh* mesh : meshes)
		mesh->draw(camera);
}

void Game::createTexturedBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	const std::string& tex_name,
	const std::string& uv_filepath
) {
	Mesh* box = Mesh::makeBox(name, size.z, size.x, size.y, pos);
	box->texture = getTextureByName(tex_name);
	box->useUVMap(uv_filepath);
	box->attachShader(getShaderByName("texture_shader"));

	meshes.push_back(box);
}

void Game::createColoredBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	glm::vec3 color
) {
	Mesh* box = Mesh::makeBox(name, size.x, size.y, size.z, pos);
	box->useColorMode(color);
	box->attachShader(getShaderByName("color_shader"));

	meshes.push_back(box);
}