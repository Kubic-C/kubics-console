// define two managers of which manage a 
// buffer.
#pragma once
#ifndef KUBICS_CONSOLE_KCONSOLE_BUFFER_H
#define KUBICS_CONSOLE_KCONSOLE_BUFFER_H

#include "kconsole_shader.hpp"

#define WAIT_FOR_ENTER -1
#define AT_CURSOR -1
#define HIDE_INPUT -2

// copied from glfw3.h file
#define KC_KEY_UNKNOWN            -1
#define KC_KEY_SPACE              32
#define KC_KEY_APOSTROPHE         39  /* ' */
#define KC_KEY_COMMA              44  /* , */
#define KC_KEY_MINUS              45  /* - */
#define KC_KEY_PERIOD             46  /* . */
#define KC_KEY_SLASH              47  /* / */
#define KC_KEY_0                  48
#define KC_KEY_1                  49
#define KC_KEY_2                  50
#define KC_KEY_3                  51
#define KC_KEY_4                  52
#define KC_KEY_5                  53
#define KC_KEY_6                  54
#define KC_KEY_7                  55
#define KC_KEY_8                  56
#define KC_KEY_9                  57
#define KC_KEY_SEMICOLON          59  /* ; */
#define KC_KEY_EQUAL              61  /* = */
#define KC_KEY_A                  65
#define KC_KEY_B                  66
#define KC_KEY_C                  67
#define KC_KEY_D                  68
#define KC_KEY_E                  69
#define KC_KEY_F                  70
#define KC_KEY_G                  71
#define KC_KEY_H                  72
#define KC_KEY_I                  73
#define KC_KEY_J                  74
#define KC_KEY_K                  75
#define KC_KEY_L                  76
#define KC_KEY_M                  77
#define KC_KEY_N                  78
#define KC_KEY_O                  79
#define KC_KEY_P                  80
#define KC_KEY_Q                  81
#define KC_KEY_R                  82
#define KC_KEY_S                  83
#define KC_KEY_T                  84
#define KC_KEY_U                  85
#define KC_KEY_V                  86
#define KC_KEY_W                  87
#define KC_KEY_X                  88
#define KC_KEY_Y                  89
#define KC_KEY_Z                  90
#define KC_KEY_LEFT_BRACKET       91  /* [ */
#define KC_KEY_BACKSLASH          92  /* \ */
#define KC_KEY_RIGHT_BRACKET      93  /* ] */
#define KC_KEY_GRAVE_ACCENT       96  /* ` */
#define KC_KEY_WORLD_1            161 /* non-US #1 */
#define KC_KEY_WORLD_2            162 /* non-US #2 */
#define KC_KEY_ESCAPE             256
#define KC_KEY_ENTER              257
#define KC_KEY_TAB                258
#define KC_KEY_BACKSPACE          259
#define KC_KEY_INSERT             260
#define KC_KEY_DELETE             261
#define KC_KEY_RIGHT              262
#define KC_KEY_LEFT               263
#define KC_KEY_DOWN               264
#define KC_KEY_UP                 265
#define KC_KEY_PAGE_UP            266
#define KC_KEY_PAGE_DOWN          267
#define KC_KEY_HOME               268
#define KC_KEY_END                269
#define KC_KEY_CAPS_LOCK          280
#define KC_KEY_SCROLL_LOCK        281
#define KC_KEY_NUM_LOCK           282
#define KC_KEY_PRINT_SCREEN       283
#define KC_KEY_PAUSE              284
#define KC_KEY_F1                 290
#define KC_KEY_F2                 291
#define KC_KEY_F3                 292
#define KC_KEY_F4                 293
#define KC_KEY_F5                 294
#define KC_KEY_F6                 295
#define KC_KEY_F7                 296
#define KC_KEY_F8                 297
#define KC_KEY_F9                 298
#define KC_KEY_F10                299
#define KC_KEY_F11                300
#define KC_KEY_F12                301
#define KC_KEY_F13                302
#define KC_KEY_F14                303
#define KC_KEY_F15                304
#define KC_KEY_F16                305
#define KC_KEY_F17                306
#define KC_KEY_F18                307
#define KC_KEY_F19                308
#define KC_KEY_F20                309
#define KC_KEY_F21                310
#define KC_KEY_F22                311
#define KC_KEY_F23                312
#define KC_KEY_F24                313
#define KC_KEY_F25                314
#define KC_KEY_KP_0               320
#define KC_KEY_KP_1               321
#define KC_KEY_KP_2               322
#define KC_KEY_KP_3               323
#define KC_KEY_KP_4               324
#define KC_KEY_KP_5               325
#define KC_KEY_KP_6               326
#define KC_KEY_KP_7               327
#define KC_KEY_KP_8               328
#define KC_KEY_KP_9               329
#define KC_KEY_KP_DECIMAL         330
#define KC_KEY_KP_DIVIDE          331
#define KC_KEY_KP_MULTIPLY        332
#define KC_KEY_KP_SUBTRACT        333
#define KC_KEY_KP_ADD             334
#define KC_KEY_KP_ENTER           335
#define KC_KEY_KP_EQUAL           336
#define KC_KEY_LEFT_SHIFT         340
#define KC_KEY_LEFT_CONTROL       341
#define KC_KEY_LEFT_ALT           342
#define KC_KEY_LEFT_SUPER         343
#define KC_KEY_RIGHT_SHIFT        344
#define KC_KEY_RIGHT_CONTROL      345
#define KC_KEY_RIGHT_ALT          346
#define KC_KEY_RIGHT_SUPER        347
#define KC_KEY_MENU               348
#define KC_KEY_LAST               GLFW_KEY_MENU

// settings 
#define invoke_method(object, fun_ptr) ((*object.*fun_ptr)())

#define KC_WRAPPING 0x01
#define KC_BUFFER_WRAPPING 0x02

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

	/* input_manager
	 * impl for input manager
	 * not supposed to be created
	 * by itself
	*/
	class input_manager
	{
	public:
		input_manager();

	public:
		// get the input buffer
		void get_in_buf(
			std::wstring& str
		);

		// clear the input buffer
		void clear_input_buffer();

		// set the callbacks to a window
		void set_callbacks(
			void* window
		);

		// see if the key is pressed
		bool key_pressed(
			void* window,
			int key
		);

	protected:
		std::wstring in_buf;
		std::vector<key_event> keyboard_events;
		
	};

	/* output_manager
	 * impl for output manager
	 * not supposed to be created
	 * by itself
	*/
	class output_manager
	{
	public:
		output_manager();
		~output_manager();

		// specify screen buffer size
		output_manager(
			int width,
			int height
		);

		// initialize members post-constructor
		void construct(
			int width,
			int height
		);

	public:
		// render everything in the buffer
		void render_buf();

		// use a new font(allocated on the heap)
		// once in use. the output_manager will take
		// care of deletion
		void use_font(
			font* font_ptr
		);

		// use a new font from a dir
		void use_font(
			const char* font_dir,
			bool& isgood,
			uint32_t font_size = 48,
			uint32_t loading_range = 128
		);

		// use a program
		void use_program(
			gl_program* program
		);

		// construct a new program, returns true if dirs were good and compilation and linking was good
		bool use_program(
			std::vector<std::string>& errors,
			const char* vertex_source_dir,
			const char* frag_source_dir
		);

		// write to the console output buffer using a 2D buffer
		// the argument buf MUST have the size of output_buffer
		void write2D(
			wchar_t** buf
		);

		// write to the console output buffer using a 1D buffer
		void write1D(
			wchar_t* buf,
			int buf_size
		);

		// set the color between to pos
		void set_color(
			glm::ivec2 pos1,
			glm::ivec2 pos2, 
			glm::vec3 color
		);

		// set the dim of the virtual screen
		// this will affect wrapping
		void set_virtual_screen(
			float width,
			float height
		);

		// return true if "setting" arg was valid
		bool set_output_setting(
			uint32_t setting,
			bool enable
		);

		// delete anything stored on the GPU
		void delete_();

		// clear the output buffer
		void clear_output_buffer();

	protected:
		uint32_t vertex_array;
		uint32_t vertex_buffer;
		uint32_t indices_buffer;
		glm::ivec2 cell_dim;
		glm::vec2 draw_pos;
		glm::vec2 cur_draw_pos;
		glm::vec2 screen_dim;
		glm::fmat4 screen_mat;
		std::vector<float> vertice_buffer;
		std::vector<std::vector<cell>> output_buffer;
		font* current_font;
		gl_program* program;

		void delete_font();

		void delete_program();

		void _generate_cells();

		void init_members();

		// settings
		
		typedef void(output_manager::*out_setting_fun)();
		typedef bool(output_manager::*out_setting_bool_fun)();
		void do_nothing() {}
		bool do_bool_nothing() { return false;  }

		// return true if draw cursor is off screen
		bool wrap();
		out_setting_bool_fun wrap_setting;

		void buffer_wrap();
		out_setting_fun buffer_wrap_setting;
	};
}

#endif // KUBICS_CONSOLE_KCONSOLE_BUFFER_H