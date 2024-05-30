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
	glGetProgramiv(this->ID, GL_LINK_STATUS,& success);
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

void Shader::setVec3(const std::string& name, glm::vec3 vec) const
{
	glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::getUVMap(const std::string& filepath, std::vector<glm::vec2>& coords)
{
	std::ifstream file;
	std::string line;

	file.open(filepath);
	if (!file.is_open())
	{
		std::cout << "UV file at \"" << filepath << "\" not found" << std::endl;
		return;
	}

	while (std::getline(file, line))
	{
		std::stringstream buffer(line);
		glm::vec2 coord;
		buffer >> coord.x;
		buffer >> coord.y;

		coords.push_back(coord);
	}

	file.close();
}

void Shader::makeVAOFromTris(
	const std::vector<glm::vec3>& verts,
	VAOStyle style,
	unsigned int& VAO,
	unsigned int& VBO,
	const std::string& UV_filepath,
	const glm::vec3& color
) {
	int vertex_count = 0;
	std::vector<float> data;

	switch (style)
	{
	case TEXTURED:
	{
		std::vector<glm::vec2> uv_coords;
		getUVMap(UV_filepath, uv_coords);

		if (verts.size() != uv_coords.size())
		{
			std::cout << "Vertex and UV coordinate size mismatch (";
			std::cout << verts.size() << ", " << uv_coords.size() << "), aborting VAO creation" << std::endl;
			break;
		}

		for (int vert = 0; vert < verts.size(); vert++)
		{
			for (int i = 0; i < 3; i++)
				data.push_back(verts[vert][i]);

			data.push_back(uv_coords[vert].x);
			data.push_back(uv_coords[vert].y);
			
			vertex_count += 3;
		}

		glGenVertexArrays(1,& VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1,& VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

		int stride = 5 * sizeof(float);

		//position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);

		// texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		
		break;
	}
	case SOLID_COLORED:
	{
		for (const glm::vec3& vert : verts)
		{
			for (int i = 0; i < 3; i++)
					data.push_back(vert[i]);

			vertex_count += 3;
		}

		glGenVertexArrays(1,& VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1,& VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

		int stride = 3 * sizeof(float);

		//position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);

		break;
	}
	}
}

void Shader::makeVAOFromVertsFaces(
	const std::vector<glm::vec3>& verts,
	const std::vector<int>& faces,
	VAOStyle style,
	unsigned int& VAO,
	unsigned int& VBO,
	const std::string& UV_filepath,
	const glm::vec3& color
) {

}

unsigned int Shader::makeModule(
	const std::string& filepath, unsigned int module_type)
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
	glShaderSource(shader_module, 1,& shader_src, NULL);
	glCompileShader(shader_module);

	int success;
	glGetShaderiv(shader_module, GL_COMPILE_STATUS,& success);
	if (!success)
	{
		char error_log[1024];
		glGetShaderInfoLog(shader_module, 1024, NULL, error_log);
		std::cout << "Shader Module \"" << filepath << "\" compilation error : \n" << error_log << std::endl;
	}

	return shader_module;
}
