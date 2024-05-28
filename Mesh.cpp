#include "Mesh.h"

Mesh::Mesh(const std::string& name, std::vector<Tri>& tris)
{
	this->name = name;
	for (Tri tri : tris)
		this->tris.push_back(tri);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Mesh::draw(Camera* camera)
{
	shader->use();
	shader->setMat4("projection", camera->getProjectionMat());
	shader->setMat4("transform", transform.getMat());

	switch (shader_mode)
	{
	case TEXTURE:
		glBindTexture(GL_TEXTURE_2D, texture);
		break;

	case COLOR:
		shader->setVec3("color", color);
		break;
	}
		
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void Mesh::attachShader(Shader* shader)
{
	this->shader = shader;
}


void Mesh::useUVMap(const std::string& filepath)
{
	std::ifstream file;
	std::string line;

	file.open(filepath);
	if (!file.is_open())
	{
		std::cout << "UV file at \"" << filepath << "\" not found" << std::endl;
		return;
	}

	int t = 0;
	for (Tri& tri : tris)
	{
		std::cout << "tri " << t++ << std::endl;
		for (int vert = 0; vert < 3; vert++)
		{
			if (!std::getline(file, line))
				break;
			std::stringstream buffer;
			buffer << line;
			buffer >> tri.uv_coords[vert][0];
			buffer >> tri.uv_coords[vert][1];

			//std::cout << "vert " << vert << " uv: " << tri.uv_coords[vert][0] << ", " << tri.uv_coords[vert][1] << std::endl;
		}
	}

	file.close();
	updateVAO();
}

void Mesh::useTextureMode()
{
	shader_mode = TEXTURE;
	updateVAO();
}

void Mesh::useTextureMode(unsigned int texture)
{
	this->texture = texture;
}

void Mesh::useColorMode()
{
	shader_mode = COLOR;
	updateVAO();
}

void Mesh::useColorMode(const glm::vec3& color)
{
	this->color = color;
	useColorMode();
}

void Mesh::updateVAO()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	switch (shader_mode)
	{
	case TEXTURE:
		createTextureVAO();
		break;
	case COLOR:
		createSolidColorVAO();
		break;
	}
}

void Mesh::createTextureVAO()
{
	vertex_count = 0;
	std::vector<float> data;

	for (Tri& tri : tris)
	{
		for (int vert = 0; vert < 3; vert++)
		{
			for (int i = 0; i < 3; i++)
				data.push_back(tri.verts[vert][i]);
			data.push_back(tri.uv_coords[vert][0]);
			data.push_back(tri.uv_coords[vert][1]);
		}

		vertex_count += 3;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	int stride = 5 * sizeof(float);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// texture coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Mesh::createSolidColorVAO()
{
	vertex_count = 0;
	std::vector<float> data;

	for (Tri& tri : tris)
	{
		for (int vert = 0; vert < 3; vert++)
		{
			for (int i = 0; i < 3; i++)
				data.push_back(tri.verts[vert][i]);
		}

		vertex_count += 3;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	int stride = 3 * sizeof(float);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
}

Mesh* Mesh::makePlane(const std::string& name, float l, float w, const glm::vec3& pos)
{
	glm::vec3 v000(0, 0, 0);
	glm::vec3 v00l(0, 0, l);
	glm::vec3 vw00(w, 0, 0);
	glm::vec3 vw0l(w, 0, l);

	std::vector<Mesh::Tri> tris{
		Mesh::Tri(v000, v00l, vw0l),
		Mesh::Tri(v000, vw0l, vw00)
	};

	glm::vec3 pos_ = pos;
	pos_.x -= w / 2;
	pos_.z -= l / 2;

	Mesh* mesh = new Mesh(name, tris);
	mesh->transform.translate(pos_);
	return mesh;
}

Mesh* Mesh::makeBox(const std::string& name, float l, float w, float h, const glm::vec3& pos)
{
	glm::vec3 v000(0.0f, 0.0f, 0.0f);
	glm::vec3 v00l(0.0f, 0.0f, l);
	glm::vec3 v0h0(0.0f, h, 0.0f);
	glm::vec3 v0hl(0.0f, h, l);
	glm::vec3 vw00(w, 0.0f, 0.0f);
	glm::vec3 vw0l(w, 0.0f, l);
	glm::vec3 vwh0(w, h, 0.0f);
	glm::vec3 vwhl(w, h, l);

	std::vector<Mesh::Tri> tris{
		//-x face
		Mesh::Tri(v000, v00l, v0h0),
		Mesh::Tri(v00l, v0hl, v0h0),
		//+z face
		Mesh::Tri(v00l, vw0l, v0hl),
		Mesh::Tri(vw0l, vwhl, v0hl),
		//+x face
		Mesh::Tri(vw0l, vw00, vwhl),
		Mesh::Tri(vw00, vwh0, vwhl),
		//-z face
		Mesh::Tri(vw00, v000, vwh0),
		Mesh::Tri(v000, v0h0, vwh0),
		//+y face
		Mesh::Tri(v0h0, v0hl, vwh0),
		Mesh::Tri(v0hl, vwhl, vwh0),
		//-y face
		Mesh::Tri(vw00, vw0l, v000),
		Mesh::Tri(vw0l, v00l, v000)
	};

	//pos.x -= w / 2;
	//pos.y -= h / 2;
	//pos.z -= l / 2;

	Mesh* mesh = new Mesh(name, tris);
	mesh->transform.translate(pos);
	return mesh;
}