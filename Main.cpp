#include "config.h"
#include "util.h"
#include "TriangleMesh.h"

unsigned int makeShader(const std::string& vertex_filepath, const std::string& fragment_filepath);
unsigned int makeModule(const std::string& filepath, unsigned int module_type);

int main()
{
	if (!glfwInit())	// MUST RUN BEFORE GLAD INIT
	{
		std::cout << "GLFW couldn't start" << std::endl;
		return -1;
	}
	GLFWwindow* window;
	
	window = glfwCreateWindow(640, 480, "Voxel Game", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	// MUST RUN BEFORE ANY OTHER OPENGL FUNCTION
	{
		glfwTerminate();
		std::cout << "Glad couldn't start" << std::endl;
		return -1;
	}
	//printFileToTerminal("shaders/fragment.txt", true);

	TriangleMesh* tri = new TriangleMesh();

	glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

	unsigned int shader = makeShader(
		"shaders/vertex.txt",
		"shaders/fragment.txt"
	);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		tri->draw();
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}

unsigned int makeShader(const std::string& vertex_filepath, const std::string& fragment_filepath)
{
	std::vector<unsigned int> modules;
	modules.push_back(makeModule(vertex_filepath, GL_VERTEX_SHADER));
	modules.push_back(makeModule(fragment_filepath, GL_FRAGMENT_SHADER));

	unsigned int shader = glCreateProgram();
	for (unsigned int shader_module : modules)
	{
		glAttachShader(shader, shader_module);
	}
	glLinkProgram(shader);

	int success;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success)
	{
		char error_log[1024];
		glGetProgramInfoLog(shader, 1024, NULL, error_log);
		std::cout << "Shader Linking error:\n" << error_log << std::endl;
	}

	for (unsigned int shader_module : modules)
	{
		glDeleteShader(shader_module);
	}

	return shader;
}

unsigned int makeModule(const std::string& filepath, unsigned int module_type)
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
		std::cout << "Shader Module compilation error:\n" << error_log << std::endl;
	}

	return shader_module;
}
