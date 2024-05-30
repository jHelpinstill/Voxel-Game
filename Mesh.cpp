#include "Mesh.h"

Mesh::Mesh(
	const std::string& name,
	const std::vector<glm::vec3>& verts,
	unsigned int texture,
	const std::string& uv_filepath
) {
	this->name = name;
	this->texture = texture;
	this->uv_filepath = uv_filepath;

	for (const glm::vec3& vert : verts)
		this->verts.push_back(vert);
	getUVMap(uv_filepath);

	style = Shader::VAOStyle::TEXTURED;
}

Mesh::Mesh(
	const std::string& name,
	const std::vector<glm::vec3>& verts,
	unsigned int texture,
	const std::vector<glm::vec2>& uv_coords
) {
	this->name = name;
	this->texture = texture;

	for (const glm::vec3& vert : verts)
		this->verts.push_back(vert);
	for (const glm::vec2& uv_coord : uv_coords)
		this->uv_coords.push_back(uv_coord);

	style = Shader::VAOStyle::TEXTURED;
}

Mesh::Mesh(
	const std::string& name,
	const std::vector<glm::vec3>& verts,
	glm::vec3 color
) {
	this->name = name;
	this->color = color;

	for (const glm::vec3& vert : verts)
		this->verts.push_back(vert);

	style = Shader::VAOStyle::SOLID_COLORED;
}

Mesh::~Mesh()
{
	deleteVAO();
}

void Mesh::draw(Camera* camera)
{
	shader->use();
	shader->setMat4("projection", camera->getProjectionMat());
	shader->setMat4("transform", transform.getMat());

	switch (style)
	{
	case Shader::VAOStyle::TEXTURED:
		glBindTexture(GL_TEXTURE_2D, texture);
		break;

	case Shader::VAOStyle::SOLID_COLORED:
		shader->setVec3("color", color);
		break;
	}
		
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, verts.size());
}

void Mesh::attachShader(Shader* shader)
{
	this->shader = shader;

	shader->makeVAOFromTris(
		verts,
		style,
		VAO,
		VBO,
		uv_coords,
		color
	);
}

void Mesh::deleteVAO()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Mesh::getUVMap(const std::string& filepath)
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

		uv_coords.push_back(coord);
	}

	file.close();
}

Mesh* Mesh::makePlane(
	const std::string& name,
	const glm::vec2& size,
	unsigned int texture,
	const std::string& uv_filepath,
	const glm::vec3& pos,
	const glm::vec3& color
) {
	float w, l;
	w = size.x;
	l = size.y;

	glm::vec3 v000(0, 0, 0);
	glm::vec3 v00l(0, 0, l);
	glm::vec3 vw00(w, 0, 0);
	glm::vec3 vw0l(w, 0, l);

	std::vector<glm::vec3> verts{
		v000, v00l, vw0l,
		v000, vw0l, vw00
	};

	glm::vec3 pos_ = pos;
	pos_.x -= w / 2;
	pos_.z -= l / 2;

	Mesh* mesh;
	if (texture)
		mesh = new Mesh(name, verts, texture, uv_filepath);
	else
		mesh = new Mesh(name, verts, color);

	mesh->transform.translate(pos_);
	return mesh;
}

Mesh* Mesh::makeBox(
	const std::string& name,
	const glm::vec3& size,
	unsigned int texture,
	const std::string& uv_filepath,
	const glm::vec3& pos,
	const glm::vec3& color
) {
	float w, h, l;
	w = size.x;
	h = size.y;
	l = size.z;

	glm::vec3 v000(0.0f, 0.0f, 0.0f);
	glm::vec3 v00l(0.0f, 0.0f, l);
	glm::vec3 v0h0(0.0f, h, 0.0f);
	glm::vec3 v0hl(0.0f, h, l);
	glm::vec3 vw00(w, 0.0f, 0.0f);
	glm::vec3 vw0l(w, 0.0f, l);
	glm::vec3 vwh0(w, h, 0.0f);
	glm::vec3 vwhl(w, h, l);

	std::vector<glm::vec3> verts{
		//-x face					
		v000, v00l, v0h0,	
		v00l, v0hl, v0h0,	
		//+z face					
		v00l, vw0l, v0hl,	
		vw0l, vwhl, v0hl,	
		//+x face					
		vw0l, vw00, vwhl,	
		vw00, vwh0, vwhl,	
		//-z face					
		vw00, v000, vwh0,	
		v000, v0h0, vwh0,	
		//+y face					
		v0h0, v0hl, vwh0,	
		v0hl, vwhl, vwh0,	
		//-y face					
		vw00, vw0l, v000,	
		vw0l, v00l, v000		
	};

	Mesh* mesh;
	if (texture)
		mesh = new Mesh(name, verts, texture, uv_filepath);
	else
		mesh = new Mesh(name, verts, color);

	mesh->transform.translate(pos);
	return mesh;
}