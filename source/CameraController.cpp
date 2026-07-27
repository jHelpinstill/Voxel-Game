#include "CameraController.h"
#include "World.h"

CameraController::CameraController(Camera &camera, Input &input) {
	this->camera = &camera;
	this->input = &input;
	velocity = glm::vec3(0, 0, 0);
}

void CameraController::update(World &world, float dt) {
	if (constrain_up) {
		camera->transform.rotate(input->mouse.delta.x * mouse_sensitivity / 1000.0, up_vec, false);
		camera->transform.rotate(input->mouse.delta.y * mouse_sensitivity / 1000.0, glm::vec3(1, 0, 0), true);

		glm::vec3 move = getInputVector();
		move.x *= -1;
		move.z *= -1;

		move *= move_speed * dt;

		glm::mat4 inv_view = glm::inverse(camera->transform.view);

		
		glm::vec3 move_up = move.y * up_vec + velocity * dt;
		glm::vec3 move_fwd = glm::cross(glm::vec3(inv_view * glm::vec4(1, 0, 0, 1)), up_vec) * move.z;
		glm::vec3 move_left = inv_view * glm::vec4(move.x, 0, 0, 1);
		
		camera->transform.translate(move_up + move_left + move_fwd);
		velocity += glm::vec3(0, -9.8 * dt, 0);
		checkGround(world);
	}
	else {
		camera->transform.rotate(glm::vec3(input->mouse.delta.y * mouse_sensitivity * dt, input->mouse.delta.x * mouse_sensitivity, 0));
	}
	

	//camera->translateLocal(getInputVector() * move_speed * dt);
}

void CameraController::checkGround(World &world) {
	ChunkManager::RaycastResult cast = world.chunks.raycast(camera->transform.pos, glm::vec3(0, -1, 0));
	if(cast.hit) {
		if((camera->transform.pos - cast.pos).y < height) {
			camera->transform.pos.y = cast.pos.y + height;
			velocity.y = 0;
		}
	}
}

void CameraController::constrainLook(glm::vec3 up) {
	constrain_up = true;
	up_vec = glm::normalize(up);
}

void CameraController::freeLook() {
	constrain_up = false;
}

glm::vec3 CameraController::getInputVector() {
	glm::vec3 v(0, 0, 0);
	v.z = input->keyHeld(inputs.forward) - input->keyHeld(inputs.backward);
	v.x = input->keyHeld(inputs.left) - input->keyHeld(inputs.right);
	v.y = input->keyHeld(inputs.up) - input->keyHeld(inputs.down);

	return v;
}