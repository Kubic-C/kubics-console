// compile glsl shaders and find them in files
#pragma once
#ifndef KUBICS_CONSOLE_SHADER_H
#define KUBICS_CONSOLE_SHADER_H

#include "kconsole_font.hpp"

namespace kconsole
{
	// gl_program is OpenGL GPU program
	class gl_program
	{
	public:
		gl_program();
		~gl_program();

	public:
		// compile both shaders
		void compile(std::string& vertex_error, std::string& fragment_error, bool& vertex_isgood, bool& frag_isgood);

		// link shaders into program
		std::string link(bool& isgood);
	
		// get vertex shader source code from file
		bool set_vertex_source(const char* dir);

		// get vertex shader source code from file
		bool set_frag_source(const char* dir);

		// use the program
		void use();

		uint32_t id;

	private:
		uint32_t vertex_id;
		uint32_t frag_id;

		std::string _compile(uint32_t id, bool& isgood);
	};
}

#endif // KUBICS_CONSOLE_SHADER_H