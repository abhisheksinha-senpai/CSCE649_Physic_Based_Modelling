#include "Definitions.h"

void load_shader(std::string file_name, std::string& location)
{
    try
    {
        std::ifstream shader_file;
        shader_file.clear(std::istream::eofbit | std::istream::failbit);
        try
        {
            shader_file.open(file_name);
        }
        catch (std::ios_base::failure e) {
            std::cout << "Error: " << e.what();
            throw;
        }

        std::stringstream shader;
        shader << shader_file.rdbuf();
        shader_file.close();
        location = shader.str();
    }
    catch (std::exception e)
    {
        throw std::runtime_error(std::string("ERROR::SHADER: Failed to read shader files"));
    }
}

void create_shader_program(unsigned int &shaderProgram)
{
	std::string vertexShadersrc;
	std::string fragmentShadersrc;

	load_shader("vertex_shader.vs", vertexShadersrc);
	load_shader("fragment_shader.fs", fragmentShadersrc);


	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* src = (vertexShadersrc.c_str());
	glShaderSource(vertexShader, 1, &src, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	src = (fragmentShadersrc.c_str());
	glShaderSource(fragmentShader, 1, &src, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	std::cout << shaderProgram << std::endl;

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	}

	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}