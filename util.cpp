#include "util.h"

void printFileToTerminal(const std::string& filename, bool printname)
{
	std::ifstream file;
	std::stringstream buffered_lines;
	std::string line;

	file.open(filename);
	if (!file.is_open())
	{
		std::cout << "File \"" << filename << "\" not found" << std::endl;
		return;
	}

	if(printname) std::cout << "File \"" << filename << "\":" << std::endl;
	while (std::getline(file, line))
	{
		std::cout << line << std::endl;
	}

	file.close();
}

double getDeltaTime(double fps_limit)
{
	static double prev_time = 0;
	double dt = 0;

	if (fps_limit == 0.0)
	{
		double time = glfwGetTime();
		dt = time - prev_time;
		prev_time = time;
		return dt;
	}

	double time;
	while ((dt = (time = glfwGetTime()) - prev_time) < 1.0 / fps_limit)
		;
	prev_time = time;
	return dt;
}

std::string vec2string(const glm::vec3 vec)
{
	std::string s;
	s += std::to_string(vec.x);
	s += ", ";
	s += std::to_string(vec.y);
	s += ", ";
	s += std::to_string(vec.z);
	return s;
}

float max2(float a, float b)
{
	return a > b ? a : b;
}

// returns true if ray intersects polygon, false otherwise
bool rayIntersectsPoly(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3* verts, int num_sides, util::PolyCulling culling)
{
	glm::vec3 norm = glm::cross(verts[1] - verts[0], verts[num_sides - 1] - verts[0]);
	if (glm::dot(verts[0] - pos, norm) * glm::dot(ray, norm) < 0)
		return false;

	switch (culling)
	{
		using namespace util;
	case PolyCulling::NONE:
		break;
	case PolyCulling::CCW:
		if (glm::dot(norm, ray) >= 0)
			return false;
		break;
	case PolyCulling::CW:
		if(glm::dot(norm, ray) < 0)
			return false;
		break;
	}

	glm::vec3 leg = verts[1 % num_sides] - verts[0];
	bool sign = (glm::dot(glm::cross(ray, verts[0] - pos), leg) > 0);
	for (int i = 1; i < num_sides; i++)
	{
		leg = verts[(i + 1) % num_sides] - verts[i];
		bool next_sign = (glm::dot(glm::cross(ray, verts[i] - pos), leg) > 0);

		if (next_sign != sign)
			return false;
	}
	return true;
}

Quad::Quad(const glm::vec3& pos, int face)
{
	switch (face)
	{
	case 0:
		verts[0] = pos + util::Y;
		verts[1] = pos + util::YZ;
		verts[2] = pos + util::XYZ;
		verts[3] = pos + util::XY;
		break;
	case 1:
		verts[0] = pos;
		verts[1] = pos + util::X;
		verts[2] = pos + util::ZX;
		verts[3] = pos + util::Z;
		break;
	case 2:
		verts[0] = pos + util::X;
		verts[1] = pos + util::XY;
		verts[2] = pos + util::XYZ;
		verts[3] = pos + util::ZX;
		break;
	case 3:
		verts[0] = pos;
		verts[1] = pos + util::Z;
		verts[2] = pos + util::YZ;
		verts[3] = pos + util::Y;
		break;
	case 4:
		verts[0] = pos + util::Z;
		verts[1] = pos + util::ZX;
		verts[2] = pos + util::XYZ;
		verts[3] = pos + util::YZ;
		break;
	case 5:
		verts[0] = pos;
		verts[1] = pos + util::Y;
		verts[2] = pos + util::XY;
		verts[3] = pos + util::X;
		break;
	}
}

Quad::Quad(const glm::vec3& box_min, const glm::vec3& box_max, int face)
{
	glm::vec3 size = box_max - box_min;
	switch (face)
	{
	case 0:
		verts[0] = box_min + glm::vec3(0, size.y, 0);
		verts[1] = box_min + glm::vec3(0, size.y, size.z);
		verts[2] = box_max;
		verts[3] = box_min + glm::vec3(size.x, size.y, 0);
		break;
	case 1:
		verts[0] = box_min;
		verts[1] = box_min + glm::vec3(size.x, 0, 0);
		verts[2] = box_min + glm::vec3(size.x, 0, size.z);
		verts[3] = box_min + glm::vec3(0, 0, size.z);
		break;
	case 2:
		verts[0] = box_min + glm::vec3(size.x, 0, 0);
		verts[1] = box_min + glm::vec3(size.x, size.y, 0);
		verts[2] = box_max;
		verts[3] = box_min + glm::vec3(size.x, 0, size.z);
		break;
	case 3:
		verts[0] = box_min;
		verts[1] = box_min + glm::vec3(0, 0, size.z);
		verts[2] = box_min + glm::vec3(0, size.y, size.z);
		verts[3] = box_min + glm::vec3(0, size.y, 0);
		break;
	case 4:
		verts[0] = box_min + glm::vec3(0, 0, size.z);
		verts[1] = box_min + glm::vec3(size.x, 0, size.z);
		verts[2] = box_max;
		verts[3] = box_min + glm::vec3(0, size.y, size.z);
		break;
	case 5:
		verts[0] = box_min;
		verts[1] = box_min + glm::vec3(0, size.y, 0);
		verts[2] = box_min + glm::vec3(size.x, size.y, 0);
		verts[3] = box_min + glm::vec3(size.x, 0, 0);
		break;
	}
}

namespace util
{
	glm::vec3 X = glm::vec3(1, 0, 0);
	glm::vec3 Y = glm::vec3(0, 1, 0);
	glm::vec3 Z = glm::vec3(0, 0, 1);

	glm::vec3 XY = glm::vec3(1, 1, 0);
	glm::vec3 YZ = glm::vec3(0, 1, 1);
	glm::vec3 ZX = glm::vec3(1, 0, 1);

	glm::vec3 XYZ = glm::vec3(1, 1, 1);
}