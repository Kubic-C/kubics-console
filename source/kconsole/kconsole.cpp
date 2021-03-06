#include "headers/kconsole.hpp"

// gl
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// glm
#include <glm/gtc/type_ptr.hpp>

namespace kconsole
{
	// local data
	glm::mat4x4* cur_mat;
	uint32_t highest_glyph;
	gl_program* cur_prog;
	glm::vec2* cur_draw_pos_ptr;
	glm::vec2* cur_vscreen_dim;
	_console_impl* cur_console;

	void window_size_callback(
		GLFWwindow* window,
		int width,
		int height
	)
	{
		*cur_mat = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

		if(cur_prog)
			glUniformMatrix4fv(glGetUniformLocation(cur_prog->id, "projection"), 1, GL_FALSE, glm::value_ptr(*cur_mat));

		cur_draw_pos_ptr->y =
			static_cast<float>(height - highest_glyph);

		*cur_vscreen_dim = glm::vec2(
			static_cast<float>(width), static_cast<float>(height));

		cur_console->_draw();
	}

	void framebuffer_callback(
		GLFWwindow* window,
		int width, 
		int height
	
	)
	{
		glViewport(0, 0, width, height);
	}

}

#include <iostream>

namespace kconsole
{
	typedef std::unique_lock<std::mutex> scope_mtx;

	// MAIN //
	void _console_impl::main()
	{
		// start opengl(glfw, and glew)
		_start();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		while (!glfwWindowShouldClose((GLFWwindow*)window) && !done)
		{

			apply_args();
			_draw();
			glfwPollEvents();

			// see if the main thread requested a stop
			if (stop)
			{
				response_stop_console_thread();
			}
		}

		delete_();
		glfwTerminate();
		done = true; // if done was not yet set to true
	}

	// constructors //
	_console_impl::_console_impl(
		int width,
		int height,
		int buffer_width,
		int buffer_height,
		const std::string& window_name,
		bool& good
	)
		: done(false), error(false), error_info(), tmain(nullptr), window(nullptr),
		stop(false)
	{
		cell_dim.x = buffer_width;
		cell_dim.y = buffer_height;

		window_args_ptr = new window_args;
		window_good = &good;
		window_args_ptr->width = width;
		window_args_ptr->height = height;
		window_args_ptr->name = window_name;

		cur_vscreen_dim = &screen_dim;
		cur_console = this;
		screen_dim = glm::vec2(
			static_cast<float>(width), static_cast<float>(height));
	}

	_console_impl::~_console_impl()
	{
		if (window_args_ptr)
		{
			delete window_args_ptr;
			window_args_ptr = nullptr;
		}
	}

	// methods //
	void _console_impl::start()
	{
		if (!program_args_ptr || !font_args_ptr)
		{
			error_info.emplace_back(KC_ERROR_FONT_OR_PROGRAM_NOT_SET);
			error = true;
		}

		if (error)
			return;

		tmain = new std::thread(&_console_impl::main, this);

		// wait for use_new_font and use_new_program to be set to false
		wait_mt.wait(); // wait for window creation
		wait_mt.wait(); // wait for program and font creation, good or bad
	}

	void _console_impl::end()
	{
		if (tmain) // if tmain running
		{
			done = true;
			wait_ct.notify();
			tmain->join();
			delete tmain;
			tmain = nullptr;
		}
	}

	// output_buffer //
	void _console_impl::use_font_mtx(
		font* font
	)
	{
		thread_gaurd tg(this, true);
		use_font(font);
	}

	void _console_impl::use_font_mtx(
		const char* font_dir, 
		bool& isgood, 
		uint32_t font_size,
		uint32_t loading_range
	)
	{
		thread_gaurd tg(this, true);
		font_args_ptr = new font_args;
		font_args_ptr->isgood   = &isgood;
		font_args_ptr->dir      = font_dir;
		font_args_ptr->size     = font_size;
		font_args_ptr->loading_range = loading_range;
	}

	void _console_impl::use_program_mtx(
		gl_program* program_
	)
	{
		thread_gaurd tg(this, true);
		use_program(program_);
	} 

	void _console_impl::use_program_mtx(
		std::vector<std::string>& errors,
		const char* vertex_source_dir, 
		const char* frag_source_dir, 
		bool& isgood
	)
	{
		thread_gaurd tg(this, true);
		program_args_ptr = new program_args;
		program_args_ptr->isgood = &isgood;
		program_args_ptr->errors = &errors;
		program_args_ptr->vertex_source_dir = vertex_source_dir;
		program_args_ptr->frag_source_dir = frag_source_dir;
	}

	void _console_impl::write2D_mtx(
		wchar_t** buf
	)
	{
		thread_gaurd tg(this);
		write2D(buf);
	}

	void _console_impl::write1D_mtx(
		wchar_t* buf,
		int buf_size
	)
	{
		thread_gaurd tg(this);
		write1D(buf, buf_size);
	}

	void _console_impl::set_color_mtx(
		glm::ivec2 pos1,
		glm::ivec2 pos2,
		glm::vec3 color
	)
	{
		thread_gaurd tg(this);
		set_color(pos1, pos2, color);
	}

	// input_manager //
	void _console_impl::get_in_buf_mtx(
		std::wstring& str
	)
	{
		thread_gaurd tg(this);
		get_in_buf(str);
	}

	void _console_impl::clear_output_buffer_mtx()
	{
		thread_gaurd tg(this);
		clear_output_buffer();
	}

	void _console_impl::clear_input_buffer_mtx()
	{
		thread_gaurd tg(this);
		clear_input_buffer();
	}

	bool _console_impl::key_pressed_mtx(
		int key
	)
	{
		thread_gaurd tg(this);
		return key_pressed(window, key);
	}

	bool _console_impl::set_output_setting_mtx(
		uint32_t setting,
		bool enable
	)
	{
		thread_gaurd tg(this);
		return set_output_setting(setting, enable);
	}

	// getters //

	std::string _console_impl::get_last_error()
	{
		if (error_info.size() == 0)
			return "";

		std::string last_error = error_info.back();
		error_info.pop_back();
		return last_error;
	}

	bool _console_impl::get_done() const
	{
		return done;
	}

	// private methods //
	bool _console_impl::make_window()
	{
		if (!glfwInit())
			goto bad;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(window_args_ptr->width,
			window_args_ptr->height, window_args_ptr->name.c_str(), nullptr, nullptr);

		if (!window)
		{
			glfwTerminate();
			goto bad;
		}

		set_callbacks(window);
		glfwSetWindowSizeCallback((GLFWwindow*)window, window_size_callback);
		glfwSetFramebufferSizeCallback((GLFWwindow*)window, framebuffer_callback);
		glfwSwapInterval(1); // stop the app from eating the GPU
		glfwShowWindow((GLFWwindow*)window);

		cur_mat = &screen_mat;
		cur_prog = nullptr;
		cur_draw_pos_ptr = &draw_pos;
		return true;

	bad:
		error_info.emplace_back(KC_ERROR_WINDOW_CREATION_FAILED);
		error = true;
		glfwTerminate();
		return false;
	}

	void _console_impl::apply_args()
	{
		thread_gaurd tg(this, true);

		bool changed = false;
		if (font_args_ptr)
		{
			try_new_font();
			changed = true;
		}

		if (program_args_ptr)
		{
			try_new_program();
			changed = true;
		}

		if (changed)
		{
			wait_mt.notify();
			changed = false;
		}
	}

	void _console_impl::_start()
	{
		mtx.lock();
		// make the window
		*window_good = make_window();

		// make context current
		glfwMakeContextCurrent((GLFWwindow*)window);

		// start glew
		if (glewInit() != GLEW_OK)
		{
			error = true;
			error_info.push_back(KC_ERROR_GLEW_INIT_FAILED);
			mtx.unlock();
			return;
		}

		// enable blending for the letters
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// make GPU buffers
		construct(cell_dim.x, cell_dim.y);

		wait_mt.notify();
		mtx.unlock();
	}

	void _console_impl::_draw()
	{
		mtx.lock();
		glClear(GL_COLOR_BUFFER_BIT);

		render_buf();

		glfwSwapBuffers((GLFWwindow*)window);
		mtx.unlock();
	}

	bool _console_impl::try_new_font()
	{
		use_font(font_args_ptr->dir.c_str(),
			*font_args_ptr->isgood, font_args_ptr->size, font_args_ptr->loading_range);

		if (!*font_args_ptr->isgood)
		{
			delete font_args_ptr;
			font_args_ptr = nullptr;
			done = true;

			return false;
		}

		highest_glyph = current_font->highest_glyph;
		draw_pos.y = static_cast<uint32_t>(window_args_ptr->height) - highest_glyph;

		delete font_args_ptr;
		font_args_ptr = nullptr;
		return true;
	}

	bool _console_impl::try_new_program()
	{
		*program_args_ptr->isgood = use_program(*program_args_ptr->errors,
			program_args_ptr->vertex_source_dir.c_str(), program_args_ptr->frag_source_dir.c_str());

		if (!*program_args_ptr->isgood)
		{
			delete program_args_ptr;
			program_args_ptr = nullptr;
			done = true;

			return false;
		}

		screen_mat =
			glm::ortho(0.0f, static_cast<float>(window_args_ptr->width),
				0.0f, static_cast<float>(window_args_ptr->height));

		program->use();
		glUniformMatrix4fv(glGetUniformLocation(program->id, "projection"), 1, GL_FALSE, glm::value_ptr(screen_mat));

		cur_prog = program;

		delete program_args_ptr;
		program_args_ptr = nullptr;
		return true;
	}

	void _console_impl::response_stop_console_thread()
	{
		while(!wait_mt.waiting) {}
		wait_mt.notify();

		wait_ct.wait();
	}

	void _console_impl::request_stop_console_thread()
	{
		// expected to be called from a seperate thread
		if (std::this_thread::get_id() == tmain->get_id())
			goto bad;

		//	stop console thread
		stop = true;
		wait_mt.wait();

		return;

	bad:
		error_info.emplace_back(KC_ERROR_FUNCTION_WRONG_THREAD);
		error = true;
	}

	void _console_impl::request_continue_console_thread()
	{
		// expected to be called from a seperate thread
		if (std::this_thread::get_id() == tmain->get_id())
			goto bad;

		// continue console thread
		stop = false;
		wait_ct.notify();

		return;
		
	bad:
		error_info.emplace_back(KC_ERROR_FUNCTION_WRONG_THREAD);
		error = true;
	}
}

namespace kconsole
{
	console::console(
		int width,
		int height,
		int buffer_width,
		int buffer_height,
		const std::string& window_name,
		bool& good
	)
	{
		console_ = new _console_impl(
			width, 
			height,
			buffer_width, 
			buffer_height, 
			window_name, 
			good
		);
	}

	console::~console()
	{
		delete console_;
	}
}