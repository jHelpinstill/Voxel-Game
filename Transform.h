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

	void rotate(float angle, const glm::vec3& axis, bool local = false, bool about_origin = false);
	void rotate(const glm::vec3& euler, bool local = false, bool about_origin = false);

	void lookAt(const glm::vec3& point, const glm::vec3& up);

	void translate(const glm::vec3& trans);
	void translateLocal(const glm::vec3& trans);
};

#endif