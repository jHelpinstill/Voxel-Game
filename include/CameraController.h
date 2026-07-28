#pragma once
#ifndef CAMERA_CONTROLLER
#define CAMERA_CONTROLLER

#include "config.h"
#include "Camera.h"
#include "Input.h"
#include "World.h"

class CameraController {
private:
	Input *input;
	Camera *camera;
	
	bool constrain_up = false;
	glm::vec3 up_vec;

	struct {
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
	glm::vec3 velocity;
	bool grounded = true;
	bool flight = false;
	
	float height = 1.8;
	float jump_delta_v = 5;
	float air_acceleration = 3;
	float ground_acceleration = 10;
	float ground_deceleration = 20;

	CameraController(Camera &camera, Input &input);

	void update(World &world, float dt);

	void constrainLook(glm::vec3 up);
	void freeLook();
	void checkGround(World &world);

	glm::vec3 getInputVector();
	glm::vec3 getMovementVector();
	void handleKeyInput();
};

#endif