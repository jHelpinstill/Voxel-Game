#include "Mesh.h"

Mesh::Mesh(const std::string& name, std::vector<Tri>& tris)
{
	this->name = name;
	for (Tri tri : tris)
		this->tris.push_back(tri);
	createVAO();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Mesh::draw(unsigned int texture)
{
	shader->use();
	shader->setMat4("projection", camera->getProjectionMat());
	shader->setMat4("transform", transform.getMat());
	if(texture_mode == COLOR)
		shader->setVec3("color", color);

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void Mesh::attachShader(Shader& shader)
{
	this->shader = &shader;
}

void Mesh::attachCamera(Camera& camera)
{
	this->camera = &camera;
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

	for (Tri& tri : tris)
	{
		for (int vert = 0; vert < 3; vert++)
		{
			if (!std::getline(file, line))
				break;
			std::stringstream buffer;
			buffer << line;
			buffer >> tri.uv_coords[vert][0];
			buffer >> tri.uv_coords[vert][1];

			std::cout << "vert " << vert << "uv: " << tri.uv_coords[vert][0] << ", " << tri.uv_coords[vert][1] << std::endl;
		}
	}

	file.close();
	updateVAO();
}

void Mesh::useTextureMode()
{
	texture_mode = TEXTURE;
	updateVAO();
}

void Mesh::useColorMode()
{
	texture_mode = COLOR;
	updateVAO();
}

void Mesh::updateVAO()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	createVAO();
}

void Mesh::createVAO()
{
	vertex_count = 0;
	std::vector<float> data;

	switch (texture_mode)
	{
	case TEXTURE:
	{
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

		//color / texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		break;
	}
	case COLOR:
	{
		for (Tri& tri : tris)
		{
			for (int vert = 0; vert < 3; vert++)
			{
				for (int i = 0; i < 3; i++)
					data.push_back(tri.verts[vert][i]);
			}
			//data.push_back(tri.verts[0].x); data.push_back(tri.verts[0].y); data.push_back(tri.verts[0].z);
			//data.push_back(tri.verts[1].x); data.push_back(tri.verts[1].y); data.push_back(tri.verts[1].z);
			//data.push_back(tri.verts[2].x); data.push_back(tri.verts[2].y); data.push_back(tri.verts[2].z);

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
	}
}

Mesh* Mesh::makePlane(const std::string& name, float l, float w, glm::vec3 pos)
{
	glm::vec3 v000(0, 0, 0);
	glm::vec3 v00l(0, 0, l);
	glm::vec3 vw00(w, 0, 0);
	glm::vec3 vw0l(w, 0, l);

	std::vector<Mesh::Tri> tris{
		Mesh::Tri(v000, v00l, vw0l),
		Mesh::Tri(v000, vw0l, vw00)
	};

	pos.x -= w / 2;
	pos.z -= l / 2;

	Mesh* mesh = new Mesh(name, tris);
	mesh->transform.translate(pos);
	return mesh;
}

Mesh* Mesh::makeBox(const std::string& name, float l, float w, float h, glm::vec3 pos)
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
		Mesh::Tri(v000, vw0l, v00l),
		Mesh::Tri(v000, vw00, vw0l)
	};

	pos.x -= w / 2;
	pos.y -= h / 2;
	pos.z -= l / 2;

	Mesh* mesh = new Mesh(name, tris);
	mesh->transform.translate(pos);
	return mesh;
}