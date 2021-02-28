#include "headers/kconsole_shader.hpp"

// gl
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// standard
#include <fstream>

namespace kconsole
{
	// constructors //
	gl_program::gl_program()
	{
		vertex_id = glCreateShader(GL_VERTEX_SHADER);
		frag_id = glCreateShader(GL_FRAGMENT_SHADER);
		id = glCreateProgram();
	}

	gl_program::~gl_program()
	{
		glDeleteProgram(id);
	}

	// methods //
	void gl_program::compile(std::string& vertex_error, std::string& fragment_error, bool& vertex_isgood, bool& frag_isgood)
	{
		vertex_error = _compile(vertex_id, vertex_isgood);
		fragment_error = _compile(frag_id, frag_isgood);
	}

	std::string gl_program::link(bool& isgood)
	{

		glAttachShader(id, vertex_id);
		glAttachShader(id, frag_id);
        glLinkProgram(id);
        
		int success;
		std::string info_log;
		info_log.resize(512);
		glValidateProgram(id);
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(id, 512, 0, (GLchar*)info_log.c_str());
			isgood = false;
			return info_log;
		}

		glDeleteShader(vertex_id);
		glDeleteShader(frag_id);

		isgood = true;
        return std::string("");
	}

	bool gl_program::set_vertex_source(const char* dir)
	{
		std::ifstream file(dir);
		if (file.bad() || !file.is_open())
			return false;

		std::string source = "";
		std::string line = "";
		while (std::getline(file, line))
			source += line + '\n';

		const char* csource = source.c_str();
		glShaderSource(vertex_id, 1, &csource, nullptr);

		return true;
	}

	bool gl_program::set_frag_source(const char* dir)
	{
		std::ifstream file(dir);
		if (file.bad() || !file.is_open())
			return false;

		std::string source = "";
		std::string line = "";
		while (std::getline(file, line))
			source += line + '\n';

		const char* csource = source.c_str();
		glShaderSource(frag_id, 1, &csource, nullptr);

		return true;
	}

	void gl_program::use()
	{
		glUseProgram(id);
	}

	// private methods //
	std::string gl_program::_compile(uint32_t id, bool& isgood)
	{
		glCompileShader(id);

		int success;
		std::string info_log;
		info_log.resize(512);
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(id, 512, 0, (GLchar*)info_log.c_str());
			isgood = false;
			return info_log;
		}

		isgood = true;
		return std::string("");
	}
}