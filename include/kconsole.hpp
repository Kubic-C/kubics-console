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

#define KC_ERROR_FUNCTION_WRONG_THREAD \
	"KCONSOLE ERROR: a function or method that should not have been called on the console thread was called" 

#include "kconsole_buffer.hpp"

#ifdef _WIN32 
#define  MAIN \
WINAPI WinMain( \
_In_ HINSTANCE hInstance, \
_In_opt_ HINSTANCE hPrevInstance, \
_In_ LPSTR lpCmdLine, \
_In_ int nShowCmd \
)

#define WIN_INCLUDE <Windows.h>
#else // linux or mac
#define MAIN main(int argc, argv**)
#endif  // _WIN32

namespace kconsole
{
	typedef void* winptr;

	/* 
	 _console_impl:
	 * allows you to interact
	 * and make your own
	 * console window
	 *
	 note:
	 * use kconsole::console
	 * to instantiate a console
	 * window instead
	*/ 
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
			uint32_t font_size = 48,
			uint32_t loading_range = 128
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
		// heap allocated arrays only!
		void write2D_mtx(
			wchar_t** buf
		);

		// write to the console output buffer using a 1D buffer, thread protected with a mutex
		// heap allocated arrays only!
		void write1D_mtx(
			wchar_t* buf, 
			int buf_size
		);

		// set the color of chararacters between two points, thread protected with a mutex
		// note: this a slightly expenisve function, don't do too often;
		// one time is all you need to do make it have an affect!
		void set_color_mtx(
			glm::ivec2 pos1,
			glm::ivec2 pos2,
			glm::vec3 color
		);

		// draw everything, can only be called on console thread
		void _draw();

	public:
		// get input_from the user, thread protected with a mutex
		void get_in_buf_mtx(
			std::wstring& str
		);

		// clear the output buffer, thread protected with a mutex
		void clear_output_buffer_mtx();

		// clear the input buffer, thread protected with a mtuex
		void clear_input_buffer_mtx();

		// returns true if 'key' is being pressed, thread protected with a mutex
		bool key_pressed_mtx(
			int key
		);

		// set the output wrappings
		// returns false if the setting is invalid
		bool set_output_setting_mtx(
			uint32_t setting,
			bool enable
		);

	public:
		// get the last error
		std::string get_last_error();

		// see if the console is done
		bool get_done() const;

	protected:
		winptr       window;
		bool*		 window_good; 
		std::thread* tmain; // main console runs on this thread

		// thread safety and flags
		bool done;
		bool error;
		std::mutex mtx_cv;
		std::mutex mtx;
		std::vector<std::string> error_info;
		
		bool stop;
		halting_conditional_varible wait_ct; // wait console thread
		halting_conditional_varible wait_mt; // wait main thread
		std::condition_variable go;

		// args
		bool use_new_font;
		bool use_new_program;

		struct
		{
			font* font;
			bool* isgood;
			size_t size;
			size_t loading_range;
			std::string dir;
		} font_args;

		struct
		{
			gl_program*               program;
			std::string	              vertex_source_dir;
			std::string	              frag_source_dir;
			std::vector<std::string>* errors;
			bool*                     isgood;

		} program_args;

		struct
		{
			std::string name;
			int         width;
			int         height;
		} window_args;


		// utlity and helper functions

		bool make_window();

		void apply_args();

		void _start();

		void main();
		
		void request_stop_console_thread();

		void request_continue_console_thread();

		void response_stop_console_thread();

		// insures thread safety, i.e. no data racing
		class thread_gaurd
		{
		public:

			thread_gaurd(_console_impl* _ci, bool bypass = false)
				: _ci(_ci), bypass(bypass)
			{
				bool pass = (bypass || _ci->done);
				switch (pass)
				{
				case false:
					_ci->request_stop_console_thread();
				}
			}

			~thread_gaurd()
			{
				bool pass = (bypass || _ci->done);
				switch (pass)
				{
				case false:
					_ci->request_continue_console_thread();
				}
			}

		private:
			bool bypass;
			_console_impl* _ci;
		};
	};

	/* 
	 console:
	 * constructs and handles a console
	 * window with its respective buffers.
	 note:
	 * This is a wrapper for _console_impl,
	 * use this instead of allocating
	 * _console_impl directly.
	*/
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