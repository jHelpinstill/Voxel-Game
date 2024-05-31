#include "Camera.h"

Camera::Camera(GLFWwindow* window) : clip_near(0.1f), clip_far(100.f), fov(90.f)
{
	findAspectRatio(window);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void Camera::findAspectRatio(GLFWwindow* window)
{
	int width, height;
	glfwGetWindowSize(window,& width,& height);
	aspect_ratio = width / (float)height;
}

glm::mat4 Camera::getProjectionMat()
{
	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, clip_near, clip_far);

	return projection * transform.view * glm::translate(glm::mat4(1.0), -transform.pos);
}

glm::vec3 Camera::getLookDirection()
{
	return (glm::vec3)(glm::inverse(this->transform.view) * glm::vec4(0, 0, -1.0f, 1));
}