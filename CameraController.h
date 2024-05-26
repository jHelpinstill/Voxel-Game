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

public:
	bool constrain_up = false;
	glm::vec3 up_vec;

	float mouse_sensitivity = 0.01;

	CameraController(Camera& camera, Input& input);

	void update();

	void constrainLook(glm::vec3 up);
	void freeLook();
};

#endif