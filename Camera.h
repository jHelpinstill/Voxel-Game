#pragma once
#ifndef CAMERA_
#define CAMERA_
#include "config.h"

class Camera
{
public:
	glm::mat4 view;
	glm::vec3 pos;

	float fov;	// degrees
	float aspect_ratio;
	float clip_near;
	float clip_far;

	Camera(GLFWwindow* window);

	glm::mat4 getProjectionMat();
	glm::mat4 getTransform();

	void rotate(float angle, glm::vec3 axis, bool local = false, bool about_origin = false);
	void rotate(glm::vec3 euler, bool local = false, bool about_origin = false);

	//void rotateLocal(float angle, glm::vec3 axis);
	//void rotateLocal(glm::vec3 euler);	
	//void rotate(float angle, glm::vec3 axis);
	//void rotate(glm::vec3 euler);

	void lookAt(glm::vec3 point, glm::vec3 up);

	void translate(glm::vec3 trans);
	void translateLocal(glm::vec3 trans);
	void setPos(glm::vec3 pos);

	glm::vec3 getPos();
};

#endif
