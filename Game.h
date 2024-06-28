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
#include "ObjectManager.h"
#include "World.h"
#include "HUDElement.h"
#include "Debug.h"

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
	World world;
	std::unordered_map<std::string, HUDElement*> hud_elements;
	

	Game(GLFWwindow* window);
	~Game();

	void stateMachine(double dt);
	void drawMeshes();
	void drawUI();

};

#endif