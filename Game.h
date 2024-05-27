#pragma once
#ifndef GAME_
#define GAME_

#include "config.h"
#include "Input.h"
#include "Camera.h"
#include "Mesh.h"
#include "CameraController.h"
#include "Shader.h"

class Game
{
private:
	void setup();
public:
	GLFWwindow* window;
	Input* input;
	Camera* camera;
	CameraController* player;

	std::vector<Mesh*> meshes;
	std::vector<Shader*> shaders;

	Game(GLFWwindow* window);
	~Game();

	void stateMachine(double dt);
	void drawMeshes();

	Shader* getShaderByName(const std::string& name);
	Mesh* getMeshByName(const std::string& name);
};

#endif