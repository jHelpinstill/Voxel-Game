#include "config.h"
#include "util.h"
#include "TriangleMesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

Mesh* makeBox(float l, float w, float h, glm::vec3 pos);

int main()
{
	if (!glfwInit())	// MUST RUN BEFORE GLAD INIT
	{
		std::cout << "GLFW couldn't start" << std::endl;
		return -1;
	}
	//GLFWwindow* window;
#ifndef LOCK_FRAMERATE
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
#endif
	GLFWwindow* window = glfwCreateWindow(640, 480, "Voxel Game", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	// MUST RUN BEFORE ANY OTHER OPENGL FUNCTION
	{
		glfwTerminate();
		std::cout << "Glad couldn't start" << std::endl;
		return -1;
	}
	//printFileToTerminal("shaders/fragment.txt", true);

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

	Shader mesh_shader("shaders/MeshVertex.txt", "shaders/MeshFragment.txt");
	Camera camera(window);

	Mesh* box_mesh = makeBox(1, 1, 0.5, glm::vec3(-0.5, -0.75, -2.0));
	box_mesh->attachCamera(camera);
	box_mesh->attachShader(mesh_shader);


	double prev_time = 0;
	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		double dt = time - prev_time;
		prev_time = time;

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		camera.setRotation(0.5*sin(time), glm::vec3(0, 1, 0));
		box_mesh->draw();

#ifdef LOCK_FRAMERATE
		glfwSwapBuffers(window);
#else
		glFlush();
#endif
	}

	glfwTerminate();
	return 0;
}

Mesh* makeBox(float l, float w, float h, glm::vec3 pos)
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

	Mesh* mesh = new Mesh(tris);
	mesh->transform = glm::translate(glm::mat4(1.0f), pos);
	return mesh;
}