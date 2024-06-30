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

//void raycastTest(const glm::vec3& pos, const glm::vec3& ray)
//{
//	for (int i = 0; i < 6; i++)
//	{
//		Quad quad(glm::vec3(0, 3, 0), glm::vec3(1, 4, 1), i);
//		if (rayIntersectsPoly(pos, ray, quad.verts, 4, util::PolyCulling::CCW))
//			std::cout << "looking at box face " << i << std::endl;
//	}
//	std::cout << "" << std::endl;
//}


int data_node_called = 0;
void Game::setup()
{
	input = new Input(window);

	camera = new Camera(window);
	camera->transform.pos = glm::vec3(0, 6, 5);
	camera->clip_far = 1000;

	player = new CameraController(*camera, *input);
	player->constrainLook(glm::vec3(0, 1, 0));
	player->move_speed = 15;

	createShader("texture_shader", "shaders/meshVertex.txt", "shaders/meshFragment.txt");
	createShader("color_shader", "shaders/meshColorVertex.txt", "shaders/meshColorFragment.txt");
	createShader("chunk_shader", "shaders/chunkVertex.txt", "shaders/meshFragment.txt");
	createShader("solid_UI_shader", "shaders/UIColorVertex.txt", "shaders/UIColorFragment.txt");

	createTexture("smiley", "textures/smiley.png", true);
	createTexture("crate", "textures/crate.jpg");
	createTexture("chunk_texture", "textures/dirt_block.png", true);

	createTexturedBox("box_origin", glm::vec3(1, 1, 1), glm::vec3(0, 3, 0), "smiley");
	createTexturedBox("crate", glm::vec3(1, 1, 1), glm::vec3(-2, 3, -2), "crate", "meshes/box_two_face_UV.txt");
	createTexturedBox("ruler", glm::vec3(1, 1, 98), glm::vec3(0, 3, 2), "crate", "meshes/box_two_face_UV.txt");
	createTexturedBox("test_block", glm::vec3(0.1, 0.1, 0.1), glm::vec3(0), "crate", "meshes/box_two_face_UV.txt");

	HUDElement* crosshair = new HUDElement();
	hud_elements["crosshair"] = crosshair;
	crosshair->shader = getShaderByName("solid_UI_shader");
	crosshair->addRect(-0.007, -0.007, 0.014, 0.014);
	crosshair->createVAO();
	crosshair->color = glm::vec4(1.0, 1.0, 1.0, 1.0);

	world.setup();

	//Chunk* chunk = world.getChunk(0, 1, 0);
	//traceBVHi(chunk->faces_BVH);

	std::cout << "data node called: " << data_node_called << " times" << std::endl;

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

#define AVG_FPS_HISTORY_SIZE 100
struct
{
	double history[AVG_FPS_HISTORY_SIZE]{};
	int index = 0;
	double sum = 0;
	double avg() { return sum / AVG_FPS_HISTORY_SIZE; }
	void update(double fps)
	{
		sum += fps;
		sum -= history[index];
		history[index] = fps;
		++index %= AVG_FPS_HISTORY_SIZE;
	}
} avg_fps;

int num_meshes = 0;
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
		drawUI();

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
			std::cout << num_meshes << " meshes" << std::endl;
		}
		if (input->keyHeld(GLFW_KEY_LEFT_SHIFT))
			player->move_speed = 40;
		else
			player->move_speed = 5;

		player->update(dt);
		world.update(dt, camera, input);

		//world.sun_dir = glm::rotate(glm::mat4(1.0), glm::radians((float)(20 * dt)), glm::vec3(1, 0, 0)) * glm::vec4(world.sun_dir, 1.0);

		drawUI();
		drawMeshes();
		break;
	}
	}
}

void Game::drawMeshes()
{
	num_meshes = 0;
	for (auto& mesh : meshes)
	{
		mesh.second->draw(camera);
		num_meshes++;
	}
	//std::cout << c << "meshes" << std::endl;
}

void Game::drawUI()
{
	for (auto& hud_elem : hud_elements)
	{
		hud_elem.second->draw();
	}
}