#include "Transform.h"

Transform::Transform()
{
	view = glm::mat4(1.0);
	pos = glm::vec3(0, 0, 0);
}

glm::mat4 Transform::getMat()
{
	return glm::translate(view, pos);
}

void Transform::rotate(float angle, const glm::vec3& axis, bool local, bool about_origin)
{
	glm::vec3 axis_ = axis;
	if (local)
		axis_ = glm::inverse(view) * glm::vec4(axis_, 1);
	if (!about_origin)
	{
		view = glm::rotate(view, angle, axis_);
	}
}
void Transform::rotate(const glm::vec3& euler, bool local, bool about_origin)
{

}

void Transform::lookAt(const glm::vec3& point, const glm::vec3& up)
{
	view = glm::lookAt(pos, point, up) * glm::translate(glm::mat4(1.0), -pos);
}

void Transform::translate(const glm::vec3& trans)
{
	pos += trans;
}

void Transform::translateLocal(const glm::vec3& trans)
{
	glm::vec3 world_trans = glm::inverse(view) * glm::vec4(trans, 1);
	pos += world_trans;
}