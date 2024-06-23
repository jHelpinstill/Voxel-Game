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

// returns true if ray intersects polygon, false otherwise
bool rayIntersectsPoly(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3* verts, int num_sides)
{
	//glm::vec3 norm = glm::cross(verts[1] - verts[0], verts[num_sides - 1] - verts[0]);
	//if (glm::dot(norm, ray) >= 0)
	//	return false;

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