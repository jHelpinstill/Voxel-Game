#include "Camera.h"

Camera::Camera(GLFWwindow* window) : clip_near(0.1f), clip_far(100.f), fov(90.f)
{
	//view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	view = glm::mat4(1.0f);
	pos = glm::vec3(0, 0, 0);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	aspect_ratio = width / (float)height;
}

glm::mat4 Camera::getProjectionMat()
{
	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect_ratio, clip_near, clip_far);
	return projection * view * glm::translate(glm::mat4(1.0), -pos);
}

glm::mat4 Camera::getTransform()
{
	return view * glm::translate(glm::mat4(1.0), -pos);
}

void Camera::rotate(float angle, glm::vec3 axis, bool local, bool about_origin)
{
	if(local)
		axis = glm::inverse(view) * glm::vec4(axis, 1);
	if (!about_origin)
	{
		view = glm::rotate(view, angle, axis);
		////glm::vec3 pos = view[3];
		////view[3] = glm::vec4(0, 0, 0, 1);
		////view = glm::rotate(view, angle, axis);
		////translate(pos);
		//
		//glm::mat4 no_trans = view;
		//no_trans[3] = glm::vec4(0, 0, 0, 1);
		////no_trans = glm::rotate(no_trans, angle, axis);
		////no_trans[3] = view[3];
		////view = no_trans;
		//
		////view = glm::translate(glm::mat4(1.0), pos) * glm::rotate(no_trans, angle, axis) * glm::translate(glm::mat4(1.0), -pos) * view;
		//glm::vec3 current_pos = pos;
		//
		////translate(current_pos);
		//view = glm::rotate(view, angle, axis);
		////translate(-current_pos);
	}
}
void Camera::rotate(glm::vec3 euler, bool local, bool about_origin)
{

}
/*
void Camera::rotateLocal(float angle, glm::vec3 axis)
{
	glm::vec4 world_axis = glm::inverse(view) * glm::vec4(axis, 1);
	//glm::vec3 current_pos = view[3];

	//translate(-current_pos);
	view = glm::rotate(view, angle, glm::vec3(world_axis));
	//translate(current_pos);
}

void Camera::rotateLocal(glm::vec3 euler)
{
	glm::vec3 world_euler = glm::inverse(view) * glm::vec4(euler, 1);
	//glm::vec3 current_pos = view[3];

	//translate(-current_pos);
	rotate(world_euler);
	//translate(current_pos);
}

void Camera::rotate(float angle, glm::vec3 axis)
{
	//glm::vec3 current_pos = view[3];

	//translate(-current_pos);
	view = glm::rotate(view, angle, axis);
	//translate(current_pos);
}

void Camera::rotate(glm::vec3 euler)
{
	view = glm::rotate(view, euler.x, glm::vec3(1, 0, 0));
	view = glm::rotate(view, euler.y, glm::vec3(0, 1, 0));
	view = glm::rotate(view, euler.z, glm::vec3(0, 0, 1));
}
*/
void Camera::lookAt(glm::vec3 point, glm::vec3 up)
{
	glm::vec3 pos = view[3];
	view = glm::lookAt(pos, point, up);
}

void Camera::translate(glm::vec3 trans)
{
	pos += trans;
}

void Camera::translateLocal(glm::vec3 trans)
{
	//glm::mat4 inv_view = glm::inverse(view);
	//glm::vec3 world_trans = inv_view[0] * trans[0] + inv_view[1] * trans[1] + inv_view[2] * trans[2];
	glm::vec3 world_trans = glm::inverse(view) * glm::vec4(trans, 1);
	pos += world_trans;
}

void Camera::setPos(glm::vec3 pos)
{
	//view = glm::translate(view, pos - current_pos);
}

glm::vec3 Camera::getPos()
{
	return pos;
}