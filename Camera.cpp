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

void Camera::rotate(float angle, glm::vec3 axis)
{

}

void Camera::rotate(glm::vec3 euler)
{

}

void Camera::rotateWorld(float angle, glm::vec3 axis)
{
	view = glm::rotate(view, angle, axis);
}

void Camera::rotateWorld(glm::vec3 euler)
{
	//view = glm::rotate(view, euler.z, glm::vec3(0, 0, 1)) * glm::rotate(view, euler.y, )
	rotateWorld(euler.x, view * glm::vec4(1, 0, 0, 1));
	rotateWorld(euler.y, view * glm::vec4(0, 1, 0, 1));
	rotateWorld(euler.z, view * glm::vec4(0, 0, 1, 1));
}

void Camera::setRotationWorld(float angle, glm::vec3 axis)
{
	glm::vec3 pos = view[3];
	view = glm::rotate(glm::translate(glm::mat4(1.0f), pos), angle, axis);
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

void Camera::setPos(glm::vec3 pos)
{
	glm::vec3 current_pos = view[3];
	view = glm::translate(view, pos - current_pos);
}