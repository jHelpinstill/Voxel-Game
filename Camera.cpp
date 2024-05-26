#include "Camera.h"

Camera::Camera(GLFWwindow* window) : clip_near(0.1f), clip_far(100.f), fov(90.f)
{
	view = glm::mat4(1.0f);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	aspect_ratio = width / (float)height;
}

glm::mat4 Camera::getMat()
{
	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, clip_near, clip_far);
	return projection * view;
}

void Camera::rotateLocal(float angle, glm::vec3 axis)
{
	glm::vec4 world_axis = glm::inverse(view) * glm::vec4(axis, 1);
	glm::vec3 current_pos = view[3];

	translate(-current_pos);
	view = glm::rotate(view, angle, glm::vec3(world_axis));
	translate(current_pos);
}

void Camera::rotateLocal(glm::vec3 euler)
{
	glm::vec3 world_euler = glm::inverse(view) * glm::vec4(euler, 1);
	glm::vec3 current_pos = view[3];

	translate(-current_pos);
	rotate(world_euler);
	translate(current_pos);
}

void Camera::rotate(float angle, glm::vec3 axis)
{
	view = glm::rotate(view, angle, axis);
}

void Camera::rotate(glm::vec3 euler)
{
	view = glm::rotate(view, euler.x, glm::vec3(1, 0, 0));
	view = glm::rotate(view, euler.y, glm::vec3(0, 1, 0));
	view = glm::rotate(view, euler.z, glm::vec3(0, 0, 1));
}

void Camera::lookAt(glm::vec3 point, glm::vec3 up)
{
	glm::vec3 pos = view[3];
	view = glm::lookAt(pos, point, up);
}

void Camera::translate(glm::vec3 trans)
{
	view = glm::translate(view, trans);
}

void Camera::translateLocal(glm::vec3 trans)
{
	glm::vec3 world_trans = glm::inverse(view) * glm::vec4(trans, 1);
	view = glm::translate(view, world_trans);
}

void Camera::setPos(glm::vec3 pos)
{
	glm::vec3 current_pos = view[3];
	view = glm::translate(view, pos - current_pos);
}

glm::vec3 Camera::getPos()
{
	glm::vec3 pos = view[3];
	return pos;
}