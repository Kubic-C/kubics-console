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
	size_t highest_glyph;
	gl_program* cur_prog;
	glm::vec2* cur_draw_pos;

	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		*cur_mat = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
		if(cur_prog)
			glUniformMatrix4fv(glGetUniformLocation(cur_prog->id, "projection"), 1, GL_FALSE, glm::value_ptr(*cur_mat));

		cur_draw_pos->y = static_cast<float>(height - highest_glyph);
	}

	void framebuffer_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

}

namespace kconsole
{
	typedef std::unique_lock<std::mutex> scope_mtx;

	// MAIN //
	void _console_impl::main()
	{
		_start();

		// enter main thread
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		while (!glfwWindowShouldClose((GLFWwindow*)window) && !done)
		{
			apply_args();

			mtx.lock();
			glClear(GL_COLOR_BUFFER_BIT);

			render_buf();

			glfwSwapBuffers((GLFWwindow*)window);
			glfwPollEvents(); 
			mtx.unlock();

			// wait for any functions to get the lock
			while (wait != 0) {}
		}

		done = true; // if done was not yet set to true
		delete_();
		glfwTerminate();
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
		use_new_font(false), use_new_program(false), errors_arg(nullptr), font_arg(nullptr),
		program_arg(nullptr), font_dir_arg(""), font_isgood_arg(nullptr), prog_isgood_arg(nullptr), 
		vertex_source_dir_arg(""), frag_source_dir_arg(""), font_size_arg(0), loading_range_arg(0)
	{
		cell_dim.x = buffer_width;
		cell_dim.y = buffer_height;

		window_good = &good;
		width_arg = width;
		height_arg = height;
		name_arg = window_name;
	}

	_console_impl::~_console_impl()
	{
	}

	// methods //
	void _console_impl::start()
	{
		if (!use_new_font || !use_new_program)
		{
			error_info.emplace_back(KC_ERROR_FONT_OR_PROGRAM_NOT_SET);
			error = true;
		}

		if (error)
			return;

		tmain = new std::thread(&_console_impl::main, this);

		// wait for use_new_font and use_new_program to be set to false
		scope_mtx scope(mtx_cv);
		cond_var.wait(scope); // wait for window creation
		cond_var.wait(scope); // wait for program and font creation
	}

	void _console_impl::end()
	{
		if (tmain) // if tmain running
		{
			done = true;
			tmain->join();
			delete tmain;
			tmain = nullptr;
		}
	}

	// output_buffer //
	void _console_impl::use_font_mtx(font* font)
	{
		thread_gaurd tg(mtx, wait);
		use_font(font);
	}

	void _console_impl::use_font_mtx(const char* font_dir, bool& isgood, size_t font_size, size_t loading_range)
	{
		thread_gaurd tg(mtx, wait);
		font_isgood_arg = &isgood;
		font_dir_arg = font_dir;
		font_size_arg = font_size;
		loading_range_arg = loading_range;
		use_new_font = true;
	}

	void _console_impl::use_program_mtx(gl_program* program_)
	{
		thread_gaurd tg(mtx, wait);
		use_program(program_);
	} 

	void _console_impl::use_program_mtx(std::vector<std::string>& errors,
		const char* vertex_source_dir, const char* frag_source_dir, bool& isgood)
	{
		thread_gaurd tg(mtx, wait);
		prog_isgood_arg = &isgood;
		errors_arg = &errors;
		vertex_source_dir_arg = vertex_source_dir;
		frag_source_dir_arg = frag_source_dir;
		use_new_program = true;
	}

	void _console_impl::write2D_mtx(wchar_t** buf)
	{
		thread_gaurd tg(mtx, wait);
		write2D(buf);
	}

	void _console_impl::write1D_mtx(wchar_t* buf, int buf_size)
	{
		thread_gaurd tg(mtx, wait);
		write1D(buf, buf_size);
	}

	void _console_impl::set_color_mtx(
		glm::ivec2 pos1,
		glm::ivec2 pos2,
		glm::vec3 color
	)
	{
		thread_gaurd tg(mtx, wait);
		set_color(pos1, pos2, color);
	}

	// input_manager //
	void _console_impl::get_in_buf_mtx(std::wstring& str)
	{
		thread_gaurd tg(mtx, wait);
		get_in_buf(str);
	}

	void _console_impl::clear_mtx()
	{
		thread_gaurd tg(mtx, wait);
		clear();
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

	// private methods //
	bool _console_impl::make_window()
	{
		if (!glfwInit())
			goto bad;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width_arg, height_arg, name_arg.c_str(), nullptr, nullptr);
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
		cur_draw_pos = &draw_pos;

	good:
		return true;

	bad:
		error_info.emplace_back(KC_ERROR_WINDOW_CREATION_FAILED);
		error = true;
		glfwTerminate();
		return false;
	}

	void _console_impl::apply_args()
	{
		thread_gaurd tg(mtx, wait);
		bool changed = false;
		if (use_new_font)
		{
			use_font(font_dir_arg.c_str(),
				*font_isgood_arg, font_size_arg, loading_range_arg);
			
			if (!*font_isgood_arg)
				done = true;

			highest_glyph = current_font->atlas_height;
			draw_pos.y = height_arg - highest_glyph;
			use_new_font = false;
			changed = true;
		}

		if (use_new_program)
		{
			*prog_isgood_arg = use_program(*errors_arg,
				vertex_source_dir_arg.c_str(), frag_source_dir_arg.c_str());

			if (!*prog_isgood_arg)
				done = true;
			
			screen_mat = 
				glm::ortho(0.0f, static_cast<float>(width_arg), 0.0f, static_cast<float>(height_arg));

			program->use();
			glUniformMatrix4fv(glGetUniformLocation(program->id, "projection"), 1, GL_FALSE, glm::value_ptr(screen_mat));

			cur_prog = program;
			use_new_program = false;
			changed = true;
		}


		if (changed)
			cond_var.notify_one();
	}

	void _console_impl::_start()
	{
		mtx.lock();
		// make the window
		*window_good = make_window();
		cond_var.notify_all();

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
		mtx.unlock();
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