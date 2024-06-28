#pragma once
#include "config.h"

namespace util
{
	extern glm::vec3 X;
	extern glm::vec3 Y;
	extern glm::vec3 Z;

	extern glm::vec3 XY;
	extern glm::vec3 YZ;
	extern glm::vec3 ZX;

	extern glm::vec3 XYZ;

	enum PolyCulling
	{
		CW,
		CCW,
		NONE
	};
}

void printFileToTerminal(const std::string& filename, bool printname = false);

double getDeltaTime(double fps_limit = 0.0);

std::string vec2string(const glm::vec3 vec);

float max2(float a, float b);

// returns true if ray intersects polygon, false otherwise
bool rayIntersectsPoly(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3* verts, int num_sides, util::PolyCulling culling = util::PolyCulling::NONE);

class Quad
{
public:
	glm::vec3 verts[4];
	
	Quad() {}
	Quad(const glm::vec3& pos, int face);
	Quad(const glm::vec3& box_min, const glm::vec3& box_max, int face);
};
