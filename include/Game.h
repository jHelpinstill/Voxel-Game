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
#include "Decal.h"
#include "Debug.h"
#include "Font.h"
#include "RenderText.h"
#include "Textbox.h"

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
	

	Game(GLFWwindow* window);
	~Game();

	void stateMachine(double dt);
	void drawMeshes();
	void drawUI();

};

#endif