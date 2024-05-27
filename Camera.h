#pragma once
#ifndef CAMERA_
#define CAMERA_

#include "config.h"
#include "Transform.h"

class Camera
{
public:
	float fov;	// degrees
	float aspect_ratio;
	float clip_near;
	float clip_far;

	Transform transform;

	Camera(GLFWwindow* window);

	glm::mat4 getProjectionMat();
	void findAspectRatio(GLFWwindow* window);
};

#endif
