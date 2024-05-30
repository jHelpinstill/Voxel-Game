#pragma once
#ifndef GAME_
#define GAME_

#include "config.h"
#include "Input.h"
#include "Camera.h"
#include "Mesh.h"
#include "CameraController.h"
#include "Shader.h"
#include "util.h"

class Game
{
private:
	void setup();
	enum
	{
		PAUSED,
		RUNNING
	} state;

	struct Texture
	{
		unsigned int ID;
		std::string name;
		Texture(const std::string& name, unsigned int ID)
		{
			this->name = name;
			this->ID = ID;
		}
	};

public:
	GLFWwindow* window;
	Input* input;
	Camera* camera;
	CameraController* player;

	std::vector<Mesh*> meshes;
	std::vector<Shader*> shaders;
	std::vector<Texture*> textures;

	Game(GLFWwindow* window);
	~Game();

	void stateMachine(double dt);
	void drawMeshes();

	Shader* getShaderByName(const std::string& name);
	Mesh* getMeshByName(const std::string& name);
	unsigned int getTextureByName(const std::string& name);

	void createTexturedBox(
		const std::string& name,
		glm::vec3 size,
		glm::vec3 pos,
		const std::string& tex_filepath,
		const std::string& uv_filepath = "meshes/box_one_face_UV.txt"
	);
	void createBox(
		const std::string& name,
		glm::vec3 size,
		glm::vec3 pos,
		glm::vec3 color
	);
};

#endif