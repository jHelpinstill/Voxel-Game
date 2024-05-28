#pragma once
#include "config.h"

void printFileToTerminal(const std::string& filename, bool printname = false);

double getDeltaTime(double fps_limit = 0.0);
unsigned int createTexture(const std::string& filepath, bool alpha_channel = false);