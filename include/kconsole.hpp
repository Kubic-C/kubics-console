// make the console window and give the library user a way to access and utilize it
#pragma once
#ifndef KUBICS_CONSOLE_KCONSOLE_H
#define KUBICS_CONSOLE_KCONSOLE_H

#define KC_ERROR_WINDOW_CREATION_FAILED \
	"KCONSOLE ERROR: failed to create a window. Insure that your\n gpu drivers support OpenGL 3.3 or higher"

#define KC_ERROR_GLEW_INIT_FAILED \
	"KCONSOLE ERROR: failed to initialize glew"

#define KC_ERROR_FONT_OR_PROGRAM_NOT_SET \
	"KCONSOLE ERROR: either the font or program has not been set"

#include "kconsole_buffer.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>

namespace kconsole
{
	typedef void* winptr;

	// the console class
	// allows you to interact
	// and make your own
	// console window
	class _console_impl : 
		protected output_manager, protected input_manager
	{
	public:
		_console_impl(
			int width,
			int height, 
			int buffer_width,
			int buffer_height,
			const std::string& window_name,
			bool& good
		);

		~_console_impl();

	public:
		// start the console
		// the console will run on a different thread
		void start();

		// ends the console and cleans up any data being used
		void end();

	public:
		// set the font, thread protected with a mutex
		// the pointer should point to a font allocated on the heap
		void use_font_mtx(
			font* font
		);

		// set the font from dir, thread protected with a mutex
		void use_font_mtx(
			const char* font_dir,
			bool& isgood, 
			size_t font_size = 48,
			size_t loading_range = 128
		);

		// set/use a program, thread protected with a mutex
		// the pointer should point to a program allocated on the heap
		void use_program_mtx(
			gl_program* program
		);

		// set/use a program, thread protected with a mutex
		void use_program_mtx(
			std::vector<std::string>& errors,
			const char* vertex_source_dir,
			const char* frag_source_dir, 
			bool& isgood
		);

		// write to the console output buffer using a 2D buffer, thread protected with a mutex
		void write2D_mtx(
			wchar_t** buf
		);

		// write to the console output buffer using a 1D buffer, thread protected with a mutex
		void write1D_mtx(
			wchar_t* buf, 
			int buf_size
		);

		// set the color of chararacters between two points, thread protected with a mutex
		void set_color_mtx(
			glm::ivec2 pos1,
			glm::ivec2 pos2,
			glm::vec3 color
		);

	public:
		// get input_from the user, thread protected with a mutex
		void get_in_buf_mtx(
			std::wstring& str
		);

		// clear the input buffer, thread protected with a mutex
		void clear_mtx();

	public:
		// get the last error
		std::string get_last_error();

		// see if the console is done
		bool get_done() const { return done; }

	protected:
		winptr       window;
		bool*		 window_good;
		std::thread* tmain;

		// thread safety and flags
		bool done;
		bool error;
		std::mutex mtx_cv;
		std::mutex mtx;
		std::vector<std::string> error_info;
		std::condition_variable cond_var;

		// args
		bool use_new_font;
		bool use_new_program;

		// ptr
		font*       font_arg;
		gl_program* program_arg;

		// args
		int			width_arg;
		int			height_arg;
		std::string	name_arg;

		std::string font_dir_arg;
		bool*       font_isgood_arg; 
		size_t      font_size_arg;
		size_t      loading_range_arg;

		std::vector<std::string>* errors_arg;
		bool*                     prog_isgood_arg;
		std::string				  vertex_source_dir_arg;
		std::string				  frag_source_dir_arg;

		void wait_for_call();

		bool make_window();

		void apply_args();

		void _start();

		void main();
	};

	// a simple wrapper for _console_impl
	//
	// use this to initialize a console
	// window
	//
	// use '->' operator to access
	// console
	//
	// -- console --
	// the console class
	// allows you to interact
	// and make your own
	// console window
	class console
	{
	public:
		console(
			int width,
			int height,
			int buffer_width,
			int buffer_height,
			const std::string& window_name,
			bool& good
		);

		~console();

		_console_impl* operator->()
		{
			return console_;
		}

	protected:
		_console_impl* console_;
	};
}

#endif // KUBICS_CONSOLE_KCONSOLE_H