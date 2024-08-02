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
	camera->transform.pos = glm::vec3(0, 6, 5);
	camera->clip_far = 1000;

	player = new CameraController(*camera, *input);
	player->constrainLook(glm::vec3(0, 1, 0));
	player->move_speed = 15;

	createShader("texture_shader", "shaders/meshVertex.txt", "shaders/meshFragment.txt");
	createShader("color_shader", "shaders/meshColorVertex.txt", "shaders/meshColorFragment.txt");
	createShader("chunk_shader", "shaders/chunkVertex.txt", "shaders/meshFragment.txt");
	createShader("decal_shader", "shaders/DecalVertex.txt", "shaders/DecalFragment.txt");
	createShader("font_shader", "shaders/FontVertex.txt", "shaders/FontFragment.txt");

	createTexture("smiley", "textures/smiley.png", true);
	createTexture("crate", "textures/crate.jpg");
	createTexture("chunk_texture", "textures/dirt_block.png", true);
	createTexture("white_square", "textures/white_square.png", true);

	createTexturedBox("box_origin", glm::vec3(1, 1, 1), glm::vec3(0, 3, 0), "smiley");
	createTexturedBox("crate", glm::vec3(1, 1, 1), glm::vec3(-2, 3, -2), "crate", "meshes/box_two_face_UV.txt");
	createTexturedBox("ruler", glm::vec3(1, 1, 98), glm::vec3(0, 3, 2), "crate", "meshes/box_two_face_UV.txt");
	createTexturedBox("test_block", glm::vec3(0.1, 0.1, 0.1), glm::vec3(0), "crate", "meshes/box_two_face_UV.txt");

	fonts["arial"] = new Font("fonts/arial.ttf", 48);

	Decal* crosshair = createDecal("crosshair", "white_square", "decal_shader", glm::vec2(3), glm::vec2(0), window);
	crosshair->origin = glm::vec2(0.5f);
	crosshair->adjustment = glm::vec2(0.5f);

	Decal* textbox_test = createDecal("textbox", "white_square", "decal_shader", glm::vec2(100, 200), glm::vec2(0, 50), window);
	textbox_test->origin = glm::vec2(0.5, 0);
	textbox_test->adjustment = glm::vec2(0.5, 0);
	textbox_test->drawFunc = Textbox::drawTextbox;
	textbox_test->attached_obj = new Textbox("This is a textbox with wrapping and now there's a lot more text to try to get it to wrap", getFontByName("arial"), getShaderByName("font_shader"), 0.5, glm::vec3(0));

	Decal* pause_text = createDecal("pause_text", "white_square", "decal_shader", glm::vec2(475, 50), glm::vec2(0), window);
	pause_text->origin = glm::vec2(0.5);
	pause_text->adjustment = glm::vec2(0.5, 0.5);
	pause_text->drawFunc = Textbox::drawTextbox;
	pause_text->texture = 0;
	pause_text->attached_obj = new Textbox("Game Paused", getFontByName("arial"), getShaderByName("font_shader"), 1.5, glm::vec3(1, 0, 0));
	

	world.setup();
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
			getDecalByName("pause_text")->awake = false;
			getDecalByName("crosshair")->awake = true;
		}
		
		drawMeshes();
		drawUI();

		//renderText("Game is paused", getFontByName("arial"), getShaderByName("font_shader"), window, glm::vec2(100, 100), 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

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
			getDecalByName("pause_text")->awake = true;
			getDecalByName("crosshair")->awake = false;
		}
		if (input->keyHeld(GLFW_KEY_LEFT_SHIFT))
			player->move_speed = 40;
		else
			player->move_speed = 5;

		player->update(dt);
		world.update(dt, camera, input);

		//world.sun_dir = glm::rotate(glm::mat4(1.0), glm::radians((float)(20 * dt)), glm::vec3(1, 0, 0)) * glm::vec4(world.sun_dir, 1.0);

		drawMeshes();
		drawUI();
		break;
	}
	}
}

void Game::drawMeshes()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	for (auto& mesh : meshes)
		mesh.second->draw(camera);
}

void Game::drawUI()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	for (auto& decal : decals)
		decal.second->draw();
}