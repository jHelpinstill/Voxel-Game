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