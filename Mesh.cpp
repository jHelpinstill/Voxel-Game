#include "Mesh.h"

Mesh::Mesh(unsigned int texture, void (*drawFunction)(Mesh*, Camera*))
{
	this->texture = texture;
	this->drawFunc = drawFunction;

	vao = new VAO;
}

Mesh::Mesh(
	const std::vector<glm::vec3>& verts,
	unsigned int texture,
	const std::string& uv_filepath,
	void (*drawFunction)(Mesh*, Camera*)
) {
	this->texture = texture;
	this->drawFunc = drawFunction;

	for (const glm::vec3& vert : verts)
		this->verts.push_back(vert);
	getUVMap(uv_filepath);

	vao = new VAO;
	vao->makeTextured(verts, uv_coords);
}

Mesh::Mesh(
	const std::vector<glm::vec3>& verts,
	unsigned int texture,
	const std::vector<glm::vec2>& uv_coords,
	void (*drawFunction)(Mesh*, Camera*)
) {
	this->texture = texture;
	this->drawFunc = drawFunction;

	for (const glm::vec3& vert : verts)
		this->verts.push_back(vert);
	for (const glm::vec2& uv_coord : uv_coords)
		this->uv_coords.push_back(uv_coord);

	vao = new VAO;
	vao->makeTextured(verts, uv_coords);
}

Mesh::Mesh(
	const std::vector<glm::vec3>& verts,
	glm::vec3 color,
	void (*drawFunction)(Mesh*, Camera*)
) {
	this->color = color;
	this->drawFunc = drawFunction;

	for (const glm::vec3& vert : verts)
		this->verts.push_back(vert);

	vao = new VAO;
	vao->makeSolidColored(verts, color);
}

Mesh::~Mesh()
{
	delete vao;
}

void Mesh::draw(Camera* camera)
{
	if(drawFunc)
		drawFunc(this, camera);
}

void Mesh::attachShader(Shader* shader)
{
	this->shader = shader;
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

void Mesh::drawTriangles(Mesh* mesh, Camera* camera)
{
	mesh->shader->use();
	mesh->shader->setMat4("projection", camera->getProjectionMat() * mesh->transform.getMat());
	//mesh->shader->setMat4("transform", mesh->transform.getMat());

	switch (mesh->style)
	{
	case Shader::VAOStyle::TEXTURED:
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
		break;

	case Shader::VAOStyle::SOLID_COLORED:
		mesh->shader->setVec3("color", mesh->color);
		break;
	}

	glBindVertexArray(mesh->vao->ID);
	glDrawArrays(GL_TRIANGLES, 0, mesh->verts.size());
}

void Mesh::drawInstancedStrip(Mesh* mesh, Camera* camera)
{
	mesh->shader->use();
	mesh->shader->setMat4("projection", camera->getProjectionMat());
	mesh->shader->setMat4("transform", mesh->transform.getMat());

	switch (mesh->style)
	{
	case Shader::VAOStyle::TEXTURED:
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
		break;

	case Shader::VAOStyle::SOLID_COLORED:
		mesh->shader->setVec3("color", mesh->color);
		break;
	}

	glBindVertexArray(mesh->vao->ID);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, mesh->verts.size(), mesh->instance_data.size());
}

Mesh* Mesh::makePlane(
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
		mesh = new Mesh(verts, texture, uv_filepath);
	else
		mesh = new Mesh(verts, color);

	mesh->transform.translate(pos_);
	return mesh;
}

Mesh* Mesh::makeBox(
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
		mesh = new Mesh(verts, texture, uv_filepath);
	else
		mesh = new Mesh(verts, color);

	mesh->transform.translate(pos);
	return mesh;
}