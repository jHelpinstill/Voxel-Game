#include "Shader.h"

Shader::Shader(
	const std::string& name,
	const std::string& vertex_filepath,
	const std::string& fragment_filepath
){
	this->name = name;

	unsigned int vertex_module = makeModule(vertex_filepath, GL_VERTEX_SHADER);
	unsigned int fragment_module = makeModule(fragment_filepath, GL_FRAGMENT_SHADER);

	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertex_module);
	glAttachShader(this->ID, fragment_module);

	glLinkProgram(this->ID);

	int success;
	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char error_log[1024];
		glGetProgramInfoLog(this->ID, 1024, NULL, error_log);
		std::cout << "Shader Linking error:\n" << error_log << std::endl;
	}

	glDeleteShader(vertex_module);
	glDeleteShader(fragment_module);
}

Shader::~Shader()
{
	glDeleteProgram(this->ID);
}

void Shader::use()
{
	glUseProgram(this->ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, glm::mat4 mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec4(const std::string& name, glm::vec3 vec) const
{
	glUniform4fv(glGetUniformLocation(this->ID, name.c_str()), 1, glm::value_ptr(vec));
}

unsigned int Shader::makeModule(const std::string& filepath, unsigned int module_type)
{
	std::ifstream file;
	std::stringstream buffered_lines;
	std::string line;

	file.open(filepath);
	while (std::getline(file, line))
	{
		buffered_lines << line << "\n";
	}

	std::string shader_string = buffered_lines.str();
	const char* shader_src = shader_string.c_str();
	file.close();

	unsigned int shader_module = glCreateShader(module_type);
	glShaderSource(shader_module, 1, &shader_src, NULL);
	glCompileShader(shader_module);

	int success;
	glGetShaderiv(shader_module, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char error_log[1024];
		glGetShaderInfoLog(shader_module, 1024, NULL, error_log);
		std::cout << "Shader Module \"" << filepath << "\" compilation error : \n" << error_log << std::endl;
	}

	return shader_module;
}
