// console buffer. also allow the user to "select" characters with in the buffer
#pragma once
#ifndef KUBICS_CONSOLE_KCONSOLE_BUFFER_H
#define KUBICS_CONSOLE_KCONSOLE_BUFFER_H

#include "kconsole_shader.hpp"
#include <queue>

#define WAIT_FOR_ENTER -1
#define AT_CURSOR -1
#define HIDE_INPUT -2

namespace kconsole
{

	// "character cell"
	struct cell
	{
		wchar_t char_;
		char_data data;
		char_quad quad;
	};

	// holds data on a keyboard event
	struct key_event
	{
		int key;
		int scancode; 
		int action; 
		int mods;
	};

	class input_manager
	{
	public:
		input_manager();

	public:
		// get the input buffer
		void get_in_buf(std::wstring& str);

		// clear the input buffer
		void clear();

		// set the callbacks to a window
		void set_callbacks(void* window);

	protected:
		std::wstring in_buf;
		std::queue<key_event> keyboard_events;
		
	};

	class output_manager
	{
	public:
		output_manager();
		~output_manager();

		// specify screen buffer size
		output_manager(int width, int height);

		// initialize members post-constructor
		void construct(int width, int height);

	public:
		// render everything in the buffer
		void render_buf();

		// use a new font(allocated on the heap)
		// once in use. the output_manager will take
		// care of deletion
		void use_font(font* font_ptr);

		// use a new font from a dir
		void use_font(const char* font_dir, bool& isgood, size_t font_size = 48, size_t loading_range = 128);

		// use a program
		void use_program(gl_program* program);

		// construct a new program, returns true if dirs were good and compilation and linking was good
		bool use_program(std::vector<std::string>& errors,
			const char* vertex_source_dir, const char* frag_source_dir);

		// write to the console output buffer using a 2D buffer
		// the argument buf MUST have the size of output_buffer
		void write2D(wchar_t** buf);

		// write to the console output buffer using a 1D buffer
		void write1D(wchar_t* buf, int buf_size);

		// set the color between to pos
		void set_color(glm::ivec2 pos1, glm::ivec2 pos2, glm::vec3 color);

		// delete anything stored on the GPU
		void delete_();

	protected:
		uint32_t vertex_array;
		uint32_t vertex_buffer;
		uint32_t indices_buffer;
		glm::ivec2 cell_dim;
		glm::vec2 draw_pos;
		glm::fmat4 screen_mat;
		std::vector<float> vertice_buffer;
		std::vector<std::vector<cell>> output_buffer;
		font* current_font;
		gl_program* program;

		void delete_font();

		void delete_program();

		void _generate_cells();

		void init_members();
	};
}

#endif // KUBICS_CONSOLE_KCONSOLE_BUFFER_H