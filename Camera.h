#pragma once
#ifndef CAMERA_
#define CAMERA_
#include "config.h"

class Camera
{
public:
	glm::mat4 view;

	float fov;	// degrees
	float aspect_ratio;
	float clip_near;
	float clip_far;

	Camera(GLFWwindow* window);

	glm::mat4 getMat();
	
	void rotate(float angle, glm::vec3 axis);
	void setRotation(float angle, glm::vec3 axis);
	void lookAt(glm::vec3 point, glm::vec3 up);

	void translate(glm::vec3 trans);
	void setPos(glm::vec3 pos);
};

#endif
