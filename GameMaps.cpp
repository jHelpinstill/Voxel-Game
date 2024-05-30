#include "Game.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Shader* Game::getShaderByName(const std::string& name)
{
	return shaders.at(name);
}

Mesh* Game::getMeshByName(const std::string& name)
{
	return meshes.at(name);
}

unsigned int Game::getTextureByName(const std::string& name)
{
	return textures.at(name);
}

void Game::createShader(
	const std::string& name,
	const std::string& vertex_filepath,
	const std::string& fragment_filepath
) {
	Shader* shader = new Shader(name, vertex_filepath, fragment_filepath);
	shaders[name] = shader;
}

void Game::createTexture(const std::string& name, const std::string& filepath, bool alpha_channel)
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

	textures[name] = texture;
}