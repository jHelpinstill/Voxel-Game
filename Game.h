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

public:
	GLFWwindow* window;
	Input* input;
	Camera* camera;
	CameraController* player;

	std::unordered_map<std::string, Mesh*> meshes;
	std::unordered_map<std::string, Shader*> shaders;
	std::unordered_map<std::string, unsigned int> textures;

	Game(GLFWwindow* window);
	~Game();

	void stateMachine(double dt);
	void drawMeshes();

	Shader* getShaderByName(const std::string& name);
	Mesh* getMeshByName(const std::string& name);
	unsigned int getTextureByName(const std::string& name);

	Shader* createShader(
		const std::string& name,
		const std::string& vertex_filepath,
		const std::string& fragment_filepath
	);

	unsigned int createTexture(
		const std::string& name,
		const std::string& filepath,
		bool alpha_channel = false
	);

	// Tools for creating mesh primatives
	Mesh* createTexturedBox(
		const std::string& name,
		glm::vec3 size,
		glm::vec3 pos,
		const std::string& tex_filepath,
		const std::string& uv_filepath = "meshes/box_one_face_UV.txt"
	);
	Mesh* createBox(
		const std::string& name,
		glm::vec3 size,
		glm::vec3 pos,
		glm::vec3 color = glm::vec3(0.5, 0.5, 0.5)
	);

	Mesh* createTexturedPlane(
		const std::string& name,
		glm::vec2 size,
		glm::vec3 pos,
		const std::string& tex_filepath,
		const std::string& uv_filepath = "meshes/box_one_face_UV.txt"
	);

	Mesh* createPlane(
		const std::string& name,
		glm::vec2 size,
		glm::vec3 pos,
		glm::vec3 color = glm::vec3(0.5, 0.5, 0.5)
	);
};

#endif