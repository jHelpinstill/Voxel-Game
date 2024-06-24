#pragma once
#include "config.h"

void printFileToTerminal(const std::string& filename, bool printname = false);

double getDeltaTime(double fps_limit = 0.0);

std::string vec2string(const glm::vec3 vec);

float max2(float a, float b);

// returns true if ray intersects polygon, false otherwise
bool rayIntersectsPoly(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3* verts, int num_sides);

namespace util
{
	extern glm::vec3 X;
	extern glm::vec3 Y;
	extern glm::vec3 Z;

	extern glm::vec3 XY;
	extern glm::vec3 YZ;
	extern glm::vec3 ZX;

	extern glm::vec3 XYZ;
}