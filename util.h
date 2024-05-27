#pragma once
#include "config.h"

void printFileToTerminal(const std::string& filename, bool printname = false);

double getDeltaTime(double fps_limit = 0.0);