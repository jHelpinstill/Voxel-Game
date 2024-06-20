#pragma once
#include "config.h"

void printFileToTerminal(const std::string& filename, bool printname = false);

double getDeltaTime(double fps_limit = 0.0);

std::string vec2string(const glm::vec3 vec);