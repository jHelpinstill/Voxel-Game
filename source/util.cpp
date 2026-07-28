#include "util.h"

void printFileToTerminal(const std::string &filename, bool printname) {
	std::ifstream file;
	std::stringstream buffered_lines;
	std::string line;

	file.open(filename);
	if (!file.is_open()) {
		std::cout << "File \"" << filename << "\" not found" << std::endl;
		return;
	}

	if(printname) std::cout << "File \"" << filename << "\":" << std::endl;
	while (std::getline(file, line)) {
		std::cout << line << std::endl;
	}

	file.close();
}

double getDeltaTime(double fps_limit) {
	static double prev_time = 0;
	double dt = 0;

	if (fps_limit == 0.0) {
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

std::string vec2string(const glm::vec3 &vec) {
	std::string s;
	s += std::to_string(vec.x);
	s += ", ";
	s += std::to_string(vec.y);
	s += ", ";
	s += std::to_string(vec.z);
	return s;
}

std::string mat2string(const glm::mat4 &mat) {
	std::string s;
	for(int i = 0; i < 4; i++) {
		s += std::to_string(mat[0][i]);
		s += ",\t";
		s += std::to_string(mat[1][i]);
		s += ",\t";
		s += std::to_string(mat[2][i]);
		s += ",\t";
		s += std::to_string(mat[3][i]);
		s += "\n";
	}
	return s;
}

float max2(float a, float b) {
	return a > b ? a : b;
}

glm::vec3 getPolyNorm(const glm::vec3 *verts, int num_sides, util::PolyCulling culling) {
	glm::vec3 norm = glm::cross(verts[1] - verts[0], verts[num_sides - 1] - verts[0]);
	switch(culling) {
		case util::PolyCulling::CW:
			norm = -norm;
		case util::PolyCulling::NONE:
		case util::PolyCulling::CCW:
		default: break;
	}
	return glm::normalize(norm);
}

// returns true if ray intersects polygon, false otherwise
bool rayIntersectsPoly(const glm::vec3 &pos, const glm::vec3 &ray, const glm::vec3 *verts, int num_sides, util::PolyCulling culling) {
	glm::vec3 norm = getPolyNorm(verts, num_sides, culling);

	if (glm::dot(verts[0] - pos, norm) * glm::dot(ray, norm) < 0)
		return false; // ray is pointing away from the polygon

	if(culling != util::PolyCulling::NONE && glm::dot(norm, ray) >= 0)
		return false; // ray is looking at the polygon in the transparent direction (from the back, according to culling type)

	glm::vec3 leg = verts[1 % num_sides] - verts[0];
	bool sign = (glm::dot(glm::cross(ray, verts[0] - pos), leg) > 0);
	for (int i = 1; i < num_sides; i++) {
		leg = verts[(i + 1) % num_sides] - verts[i];
		bool next_sign = (glm::dot(glm::cross(ray, verts[i] - pos), leg) > 0);

		if (next_sign != sign)
			return false;
	}
	return true;
}

Quad q0 = {util::Y, util::YZ, util::XYZ, util::XY};
Quad q1 = {glm::vec3(0), util::X, util::ZX, util::Z};
Quad q2 = {util::X, util::XY, util::XYZ, util::ZX};
Quad q3 = {glm::vec3(0), util::Z, util::YZ, util::Y};
Quad q4 = {util::Z, util::ZX, util::XYZ, util::YZ};
Quad q5 = {glm::vec3(0), util::Y, util::XY, util::X};

Quad default_quads[6] = {
	q0, q1, q2, q3, q4, q5
};

Quad::Quad(const glm::vec3 &pos, int face) {
	*this = default_quads[face];
	for(int i = 0; i < 4; i++)
		verts[i] += pos;
}

Quad::Quad(const glm::vec3 &box_min, const glm::vec3 &box_max, int face) {
	glm::vec3 size = box_max - box_min;
	switch (face) {
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

namespace util {
	glm::vec3 X = glm::vec3(1, 0, 0);
	glm::vec3 Y = glm::vec3(0, 1, 0);
	glm::vec3 Z = glm::vec3(0, 0, 1);

	glm::vec3 XY = glm::vec3(1, 1, 0);
	glm::vec3 YZ = glm::vec3(0, 1, 1);
	glm::vec3 ZX = glm::vec3(1, 0, 1);

	glm::vec3 XYZ = glm::vec3(1, 1, 1);
}