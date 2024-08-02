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

	Shader() : ID(-1) {}
	Shader(
		const std::string& name,
		const std::string& vertex_filepath,
		const std::string& fragment_filepath		
	);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setVec3(const std::string& name, glm::vec3 vec) const;
	void setVec4(const std::string& name, glm::vec4 vec) const;
	
	~Shader();
};

#endif