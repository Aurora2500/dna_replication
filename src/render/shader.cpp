#include <iostream>
#include <vector>
#include <sstream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/ext.hpp>

#include "shader.hpp"

namespace rendering
{

static unsigned int compile_shader(unsigned int type, const std::string &source)
{
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> message(length);
		glGetShaderInfoLog(id, length, &length, message.data());
		std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cerr << length << message.data() << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int compile_program(const std::string &vertex_src, const std::string &fragment_src)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_src);
	unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_src);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

Shader::Shader(const std::string &src)
{
	std::stringstream file(src);
	std::string line;
	std::string vertex_src;
	std::string fragment_src;
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};
	ShaderType type = ShaderType::NONE;
	while (std::getline(file, line))
	{
		if (line.find("//shader ") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			switch (type)
			{
			case ShaderType::VERTEX:
				vertex_src += line + "\n";
				break;
			case ShaderType::FRAGMENT:
				fragment_src += line + "\n";
				break;
			default:
				break;
			}
		}
	}
	m_id = compile_program(vertex_src, fragment_src);
}

Shader::~Shader()
{
	glDeleteProgram(m_id);
}

Shader &Shader::operator=(Shader &&other)
{
	if(this == &other)
		return *this;
	glDeleteProgram(m_id);
	m_id = other.m_id;
	m_uniforms = std::move(other.m_uniforms);
	other.m_id = 0;
	return *this;
}

int Shader::get_location(const std::string &name)
{
	if (m_uniforms.find(name) == m_uniforms.end())
	{
		int loc = glGetUniformLocation(m_id, name.c_str());
		if (loc == -1)
			std::cerr << "Warning: uniform " << name << " doesn't exist!" << std::endl;
		m_uniforms[name] = loc;
		return loc;
	}
	int loc = m_uniforms[name];
	return loc;
}

unsigned int Shader::id() const
{
	return m_id;
}

void Shader::use() const
{
	glUseProgram(m_id);
}

void Shader::set_uniform(const std::string &name, int value)
{
	glUniform1i(get_location(name), value);
}

void Shader::set_uniform(const std::string &name, float value)
{
	glUniform1f(get_location(name), value);
}

void Shader::set_uniform(const std::string &name, const glm::mat4 &value)
{
	glUniformMatrix4fv(get_location(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_uniform(const std::string &name, const glm::vec3 &value)
{
	glUniform3f(get_location(name), value.x, value.y, value.z);
}

void Shader::set_uniform(const std::string &name, const glm::vec2 &value)
{
	glUniform2f(get_location(name), value.x, value.y);
}

void Shader::set_uniform(const std::string &name, const glm::uvec2 &value)
{
	glUniform2ui(get_location(name), value.x, value.y);
}

}