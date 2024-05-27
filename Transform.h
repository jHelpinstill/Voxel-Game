#pragma once
#ifndef TRANSFORM_
#define TRANSFORM_

#include "config.h"

class Transform
{
public:
	glm::mat4 view;
	glm::vec3 pos;

	Transform();

	glm::mat4 getMat();

	void rotate(float angle, glm::vec3 axis, bool local = false, bool about_origin = false);
	void rotate(glm::vec3 euler, bool local = false, bool about_origin = false);

	void lookAt(glm::vec3 point, glm::vec3 up);

	void translate(glm::vec3 trans);
	void translateLocal(glm::vec3 trans);
};

#endif