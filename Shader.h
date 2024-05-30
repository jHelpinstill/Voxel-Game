#pragma once
#ifndef SHADER
#define SHADER

#include "config.h"

class Shader
{
private:
	unsigned int makeModule(const std::string& filepath, unsigned int module_type);

public:
	unsigned int ID;
	std::string name;

	enum VAOStyle {
		TEXTURED,
		SOLID_COLORED
	};

	Shader() {}
	Shader(
		const std::string& name,
		const std::string& vertex_filepath,
		const std::string& fragment_filepath		
	);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4 mat) const;
	void setVec3(const std::string& name, glm::vec3 vec) const;

	void getUVMap(const std::string& filepath, std::vector<glm::vec2>& coords);
	void makeVAOFromTris(
		const std::vector<glm::vec3>& verts,
		VAOStyle style,
		unsigned int& VAO,
		unsigned int& VBO,
		const std::string& UV_filepath,
		const glm::vec3& color = glm::vec3(0, 0, 0)
	);
	void makeVAOFromVertsFaces(
		const std::vector<glm::vec3>& verts,
		const std::vector<int>& faces,
		VAOStyle style,
		unsigned int& VAO,
		unsigned int& VBO,
		const std::string& UV_filepath,
		const glm::vec3& color = glm::vec3(0, 0, 0)
	);
	
	~Shader();
};

#endif