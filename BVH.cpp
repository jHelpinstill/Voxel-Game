#include "VisibleFacesBVH.h"

template <class T>
VisibleFacesBVH<T>::Box::Box() : faces(nullptr), childA(nullptr), childB(nullptr)
{
	min = glm::vec3(std::numeric_limits<float>::infinity());
	max = glm::vec3(-std::numeric_limits<float>::infinity());
}

template <class T>
void VisibleFacesBVH<T>::Box::addDataNode(const glm::vec3& pos, T* obj)
{
	DataNode* face = new DataNode;
	face->pos = pos;
	face->block = block;
	face->chunk = chunk;

	addFace(face);
}

template <class T>
void VisibleFacesBVH<T>::Box::addDataNode(DataNode* face)
{
	face->next = faces;
	faces = face;
	expandToFit(face->pos);
}

template <class T>
void VisibleFacesBVH<T>::Box::expandToFit(const glm::vec3& pos)
{
	min = glm::min(min, pos);
	max = glm::max(max, pos + glm::vec3(1));
}

float max2(float a, float b)
{
	return a > b ? a : b;
}

template <class T>
void VisibleFacesBVH<T>::Box::split()
{
	glm::vec3 size = max - min;
	if ((int)size.x == 1 && (int)size.y == 1 && (int)size.z == 1)
		return;	// dont split if size is 1x1x1

	glm::vec3 center = (max + min) * 0.5f;
	int longest_axis = (size.x > max2(size.y, size.z)) ? 0 : (size.y > size.z ? 2 : 1);

	childA = new Box();
	childB = new Box();

	float splittingPoint = center[longest_axis];
	DataNode* face = faces;
	while (face)
	{
		DataNode* current_face = face;
		face = face->next;

		if (current_face->pos[longest_axis] < splittingPoint)
			childA->addFace(current_face);
		else
			childB->addFace(current_face);
	}

	if (!childA->faces)
	{
		delete childA;
		childA = nullptr;
	}
	if (!childB->faces)
	{
		delete childB;
		childB = nullptr;
	}

	if (childA)
		childA->split();
	if (childB)
		childB->split();
}

template <class T>
bool VisibleFacesBVH<T>::Box::hitByRay(const glm::vec3& pos, const glm::vec3& ray)
{
	glm::vec3 quad_verts[4];
	glm::vec3 box_verts[8];
	glm::vec3 size = max - min;

	using namespace util;

	box_verts[0] = min;
	box_verts[1] = min + glm::vec3(size.x, 0, 0);
	box_verts[2] = min + glm::vec3(0, size.y, 0);
	box_verts[3] = min + glm::vec3(0, 0, size.z);
	box_verts[4] = min + glm::vec3(size.x, size.y, 0);
	box_verts[5] = min + glm::vec3(0, size.y, size.z);
	box_verts[6] = min + glm::vec3(size.x, 0, size.z);
	box_verts[7] = max;

	// top
	quad_verts[0] = box_verts[2];
	quad_verts[1] = box_verts[5];
	quad_verts[2] = box_verts[7];
	quad_verts[3] = box_verts[4];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// bottom
	quad_verts[0] = box_verts[0];
	quad_verts[1] = box_verts[1];
	quad_verts[2] = box_verts[6];
	quad_verts[3] = box_verts[3];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// left
	quad_verts[0] = box_verts[1];
	quad_verts[1] = box_verts[4];
	quad_verts[2] = box_verts[7];
	quad_verts[3] = box_verts[6];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// right
	quad_verts[0] = box_verts[0];
	quad_verts[1] = box_verts[3];
	quad_verts[2] = box_verts[5];
	quad_verts[3] = box_verts[2];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// forward
	quad_verts[0] = box_verts[3];
	quad_verts[1] = box_verts[6];
	quad_verts[2] = box_verts[7];
	quad_verts[3] = box_verts[5];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// back
	quad_verts[0] = box_verts[0];
	quad_verts[1] = box_verts[1];
	quad_verts[2] = box_verts[4];
	quad_verts[3] = box_verts[2];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	return false;
}

template <class T>
bool VisibleFacesBVH<T>::raycast(const glm::vec3& pos, const glm::vec3& ray, T** obj)
{
	if (!root)
		return false;

	Box* current_box = root;

	while (current_box->childA || current_box->childB)
	{
		if (!current_box->childB)
		{
			current_box = current_box->childA;
			continue;
		}
		if (!current_box->childA)
		{
			current_box = current_box->childB;
			continue;
		}

		bool hitA = current_box->childA->hitByRay(pos, ray);
		bool hitB = current_box->childA->hitByRay(pos, ray);

		if (hitA && !hitB)
		{
			current_box = current_box->childA;
			continue;
		}
		if (hitB && !hitA)
		{
			current_box = current_box->childB;
			continue;
		}
		if (!hitA && !hitB)
			return false;
		
		// only remaining option is ray intersects both A and B

	}
}