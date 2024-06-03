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
	camera->transform.pos = glm::vec3(0, 5, 5);
	camera->clip_far = 1000;

	player = new CameraController(*camera, *input);
	player->constrainLook(glm::vec3(0, 1, 0));
	player->move_speed = 15;

	createShader("texture_shader", "shaders/meshVertex.txt", "shaders/meshFragment.txt");
	createShader("color_shader", "shaders/meshColorVertex.txt", "shaders/meshColorFragment.txt");
	//createShader("world_shader", "shaders/worldVertex.txt", "shaders/meshFragment.txt");
	createShader("chunk_shader", "shaders/chunkVertex.txt", "shaders/meshFragment.txt");

	createTexture("smiley", "textures/smiley.png", true);
	createTexture("crate", "textures/crate.jpg");
	createTexture("chunk_texture", "textures/dirt_block.png", true);

	createTexturedBox("box_origin", glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), "smiley");
	createTexturedBox("crate", glm::vec3(1, 1, 1), glm::vec3(-2, 0, -2), "crate", "meshes/box_two_face_UV.txt");
	createTexturedBox("ruler", glm::vec3(1, 1, 98), glm::vec3(0, 0, 2), "crate", "meshes/box_two_face_UV.txt");
	//createPlane("ground", glm::vec2(10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 0.7, 0));

	world.setup();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

#define AVG_FPS_HISTOR_SIZE 100
struct
{
	double history[AVG_FPS_HISTOR_SIZE]{};
	int index = 0;
	double sum = 0;
	double avg() { return sum / AVG_FPS_HISTOR_SIZE; }
	void update(double fps)
	{
		sum += fps;
		sum -= history[index];
		history[index] = fps;
		++index %= AVG_FPS_HISTOR_SIZE;
	}
} avg_fps;

void Game::stateMachine(double dt)
{
	input->update();
	double time = glfwGetTime();

	std::stringstream buf;
	avg_fps.update(1.0 / dt);
	buf << "fps: " << std::setw(7) << avg_fps.avg();
	glfwSetWindowTitle(window, buf.str().c_str());

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
		if (input->keyHeld(GLFW_KEY_LEFT_SHIFT))
			player->move_speed = 40;
		else
			player->move_speed = 5;

		player->update(dt);

		//if (input->mouse.left.held)
		//{
		//	world.updateBlock(camera->transform.pos + camera->getLookDirection() * 2.0f, BlockType::AIR);
		//	//world.generateMesh();
		//}
		//else if (input->keyHeld('E') || input->mouse.right.held)
		//{
		//	world.updateBlock(camera->transform.pos + camera->getLookDirection() * 2.0f, BlockType::DIRT);
		//	//world.generateMesh();
		//}
		if (input->mouse.left.pressed)
		{
			BlockType* block;
			glm::vec3 pos = camera->transform.pos;
			world.inspectPos(pos, &block);
			std::cout << pos.x << ", " << pos.y << ", " << pos.z << ": " << getBlockName(*block) << std::endl;
		}
		//std::cout << 1.0 / dt << " avg: " << avg_fps.avg() << std::endl;

		drawMeshes();
		break;
	}
	}
}

void Game::drawMeshes()
{
	int c = 0;
	for (auto& mesh : meshes)
	{
		mesh.second->draw(camera);
		c++;
	}
	//std::cout << c << "meshes" << std::endl;
}