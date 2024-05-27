#pragma once
#ifndef CAMERA_CONTROLLER
#define CAMERA_CONTROLLER

#include "config.h"
#include "Camera.h"
#include "Input.h"

class CameraController
{
private:
	Input* input;
	Camera* camera;
	
	bool constrain_up = false;
	glm::vec3 up_vec;

	struct
	{
		int forward = 'W';
		int backward = 'S';
		int left = 'A';
		int right = 'D';
		int up = ' ';
		int down = GLFW_KEY_LEFT_CONTROL;
	} inputs;

public:
	float move_speed = 1.0;
	float mouse_sensitivity = 2;

	CameraController(Camera& camera, Input& input);

	void update(float dt);

	void constrainLook(glm::vec3 up);
	void freeLook();

	glm::vec3 getInputVector();
};

#endif