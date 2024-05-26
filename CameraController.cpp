#include "CameraController.h"

CameraController::CameraController(Camera& camera, Input& input)
{
	this->camera = &camera;
	this->input = &input;
}

void CameraController::update()
{
	if (constrain_up)
	{
		camera->rotate(input->mouse.delta.x * mouse_sensitivity, up_vec);
		camera->rotateLocal(input->mouse.delta.y * mouse_sensitivity, glm::vec3(1, 0, 0));
	}
	else
		camera->rotateLocal(glm::vec3(input->mouse.delta.y * mouse_sensitivity, input->mouse.delta.x * mouse_sensitivity, 0));
}

void CameraController::constrainLook(glm::vec3 up)
{
	constrain_up = true;
	up_vec = up;
}

void CameraController::freeLook()
{
	constrain_up = false;
}