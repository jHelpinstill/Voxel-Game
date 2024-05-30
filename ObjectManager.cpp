#include "ObjectManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::unordered_map<std::string, Mesh*> meshes;
std::unordered_map<std::string, Shader*> shaders;
std::unordered_map<std::string, unsigned int> textures;

Shader* getShaderByName(const std::string& name)
{
	return shaders.at(name);
}

Mesh* getMeshByName(const std::string& name)
{
	return meshes.at(name);
}

unsigned int getTextureByName(const std::string& name)
{
	return textures.at(name);
}

void removeShader(const std::string& name)
{
	if (shaders.find(name) == shaders.end())
	{
		std::cout << "Tried to remove nonexistent shader: \"" << name << "\"" << std::endl;
		return;
	}

	delete shaders.at(name);
	shaders.erase(name);
}
void removeMesh(const std::string& name)
{
	if (meshes.find(name) == meshes.end())
	{
		std::cout << "Tried to remove nonexistent mesh: \"" << name << "\"" << std::endl;
		return;
	}

	delete meshes.at(name);
	meshes.erase(name);
}
void removeTexture(const std::string& name)
{
	if (textures.find(name) == textures.end())
	{
		std::cout << "Tried to remove nonexistent texture: \"" << name << "\"" << std::endl;
		return;
	}
	textures.erase(name);
}

Shader* createShader(
	const std::string& name,
	const std::string& vertex_filepath,
	const std::string& fragment_filepath
) {
	Shader* shader = new Shader(name, vertex_filepath, fragment_filepath);
	shaders[name] = shader;
	return shader;
}

unsigned int createTexture(const std::string& name, const std::string& filepath, bool alpha_channel)
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
	return texture;
}

Mesh* createTexturedBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	const std::string& tex_name,
	const std::string& uv_filepath
) {
	Mesh* box = Mesh::makeBox(name, size, getTextureByName(tex_name), uv_filepath, pos);
	box->attachShader(getShaderByName("texture_shader"));

	meshes[name] = box;
	return box;
}

Mesh* createBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	glm::vec3 color
) {
	Mesh* box = Mesh::makeBox(name, size, 0, "", pos, color);
	box->attachShader(getShaderByName("color_shader"));

	meshes[name] = box;
	return box;
}

Mesh* createTexturedPlane(
	const std::string& name,
	glm::vec2 size,
	glm::vec3 pos,
	const std::string& tex_name,
	const std::string& uv_filepath
) {
	Mesh* plane = Mesh::makePlane(name, size, getTextureByName(tex_name), uv_filepath, pos);
	plane->attachShader(getShaderByName("texture_shader"));

	meshes[name] = plane;
	return plane;
}

Mesh* createPlane(
	const std::string& name,
	glm::vec2 size,
	glm::vec3 pos,
	glm::vec3 color
) {
	Mesh* plane = Mesh::makePlane(name, size, 0, "", pos, color);
	plane->attachShader(getShaderByName("color_shader"));

	meshes[name] = plane;
	return plane;
}