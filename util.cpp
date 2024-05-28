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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int createTexture(const std::string& filepath, bool alpha_channel)
{
	stbi_set_flip_vertically_on_load(true);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		int image_type = alpha_channel ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, image_type, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture from \"" << filepath << "\"" << std::endl;
	}
	stbi_image_free(data);

	return texture;
}