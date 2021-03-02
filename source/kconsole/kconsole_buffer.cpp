#include "headers/kconsole_buffer.hpp"

// gl
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// glm
#include <glm/gtc/matrix_transform.hpp>

// standard
#include <memory>

namespace kconsole
{
	std::wstring* local_in_buf = nullptr;

	void character_callback(
		GLFWwindow* window,
		unsigned int codepoint
	)
	{
		*local_in_buf += (wchar_t)codepoint;
	}

	void key_callback(
		GLFWwindow* window,
		int key, 
		int scancode, 
		int action, 
		int mods
	)
	{
		switch (key)
		{
		case KC_KEY_BACKSPACE:
			if (local_in_buf->empty() || action == GLFW_RELEASE)
				break;

			local_in_buf->pop_back();
			break;
		}
	}
}

// input class
namespace kconsole
{
	input_manager::input_manager()
	{
		local_in_buf = &in_buf;
	}

	void input_manager::get_in_buf(
		std::wstring& str
	)
	{
		str = in_buf;
	}

	void input_manager::clear_input_buffer()
	{
		in_buf.clear();
	}

	void input_manager::set_callbacks(
		void* window
	)
	{
		glfwSetCharCallback((GLFWwindow*)window, character_callback);
		glfwSetKeyCallback((GLFWwindow*)window, key_callback);
	}

	bool input_manager::key_pressed(
		void* window, 
		int key
	)
	{
		return glfwGetKey((GLFWwindow*)window, key) != GLFW_RELEASE;
	}
}

// output class
namespace kconsole
{
	// constructors //
	output_manager::output_manager()
		: current_font(nullptr), program(nullptr), cell_dim(),
		draw_pos(0, 0), indices_buffer(0), vertex_buffer(0), 
		vertex_array(0), screen_mat(), screen_dim(0, 0),
		cur_draw_pos(draw_pos)
	{
		wrap_setting = &output_manager::wrap;
		buffer_wrap_setting = &output_manager::buffer_wrap;
	}

	output_manager::~output_manager()
	{
		if(current_font)
			delete current_font;
	}

	output_manager::output_manager(
		int width,
		int height
	)
	{
		construct(width, height);
	}

	void output_manager::construct(
		int width, 
		int height
	)
	{
		cell_dim.x = width;
		cell_dim.y = height;
		init_members();
	}

	// methods //
	void output_manager::render_buf()
	{
		if (!current_font || !program) // if null
			return;

		program->use();
		glBindVertexArray(vertex_array);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, current_font->tex_id);

		uint32_t count = 0;
		cur_draw_pos = draw_pos;
		for (int x = 0; x < cell_dim.x; x++)
		{
			for (int y = 0; y < cell_dim.y; y++)
			{
				// wrap returns true if the pos_y is outside
				// of the screen, which means any more calculating
				// will be pointless if so. 
				if (invoke_method(this, wrap_setting))
					goto render;

				// add char quad data to the cpu vertice buffer
				output_buffer[x][y].quad = cdtocq(output_buffer[x][y].data, cur_draw_pos);
				std::vector<float> tquad;
				tquad.reserve(7 * 4);
				for (int i = 0; i < 4/*num of vertices*/; i++)
				{

					tquad.insert(tquad.end(),
						&output_buffer[x][y].quad.vertex[i].vertices[0],
						&output_buffer[x][y].quad.vertex[i].vertices[2]);

					tquad.insert(tquad.end(),
						&output_buffer[x][y].quad.vertex[i].tex_coords[0],
						&output_buffer[x][y].quad.vertex[i].tex_coords[2]);

					tquad.insert(tquad.end(),
						&output_buffer[x][y].quad.vertex[i].color[0],
						&output_buffer[x][y].quad.vertex[i].color[3]);
				}

				vertice_buffer.reserve(tquad.size());
				vertice_buffer.insert(vertice_buffer.end(), tquad.begin(), tquad.end());

				count++;
				cur_draw_pos.x += static_cast<float>(output_buffer[x][y].data.advance);
			}

			invoke_method(this, buffer_wrap_setting);
		}

	render:
		// put the cpu vertice buffer into the gpu vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertice_buffer.size() * sizeof(float), &vertice_buffer[0]);
		vertice_buffer.clear();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);

		glDrawElements(GL_TRIANGLES, count * 6, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void output_manager::use_font(
		font* font_ptr
	)
	{
		current_font = font_ptr;
	}

	void output_manager::use_font(
		const char* font_dir, 
		bool& isgood, 
		uint32_t font_size, 
		uint32_t loading_range
	)
	{
		delete_font();
		current_font = new font(font_dir, isgood, font_size, loading_range);
		if (!isgood)
			delete_font();
	}

	void output_manager::use_program(
		gl_program* program
	)
	{
		delete_program();
		this->program = program;
	}

	bool output_manager::use_program(
		std::vector<std::string>& errors,
		const char* vertex_source_dir,
		const char* frag_source_dir
	)
	{
		delete_program();
		program = new gl_program;
		bool vertex_dir_good = program->set_vertex_source(vertex_source_dir);
		bool frag_dir_good = program->set_frag_source(frag_source_dir);
		if (!(vertex_dir_good && frag_dir_good))
			goto bad;

		errors.resize(3);
		bool vertex_good, frag_good, program_good;

		program->compile(errors[0], errors[1], vertex_good, frag_good);
		errors[2] = program->link(program_good);
		if (!vertex_good || !frag_good || !program_good)
			goto bad;

		return true;

	bad:
		delete_program();
		return false;
	}

	void output_manager::write2D(
		wchar_t** buf
	)
	{
		for (int x = 0; x < cell_dim.x; x++)
			for (int y = 0; y < cell_dim.y; y++)
			{
				output_buffer[x][y].char_ = buf[x][y];
				glm::vec3 color = output_buffer[x][y].data.color;
				output_buffer[x][y].data = current_font->get_char(buf[x][y]);
				output_buffer[x][y].data.color = color;
			}
	}

	void output_manager::write1D(
		wchar_t* buf,
		int buf_size
	)
	{
		for (int x = 0; x < cell_dim.x; x++)
			for (int y = 0; y < cell_dim.y; y++)
			{
				int index = (y * cell_dim.x) + x;
				if (index >= buf_size)
					goto return_;

				output_buffer[x][y].char_ = buf[index];
				glm::vec3 color = output_buffer[x][y].data.color;
				output_buffer[x][y].data = current_font->get_char(buf[index]);
				output_buffer[x][y].data.color = color;
			}

	return_:
		return;
	}

	void output_manager::set_color(
		glm::ivec2 pos1, 
		glm::ivec2 pos2,
		glm::vec3 color
	)
	{
		for (int x = 0; x < cell_dim.x && x < pos2.x; x++)
			for (int y = 0; y < cell_dim.y && y < pos2.y; y++)
				if ((x >= pos1.x && y >= pos1.y) && (pos2.x >= x && pos2.y >= y))
					output_buffer[x][y].data.color = color;
	}

	void output_manager::delete_()
	{
		if(program)
			delete program;

		glDeleteVertexArrays(1, &vertex_array);
		glDeleteBuffers(1, &vertex_buffer);
		glDeleteBuffers(1, &indices_buffer);
	}

	void output_manager::clear_output_buffer()
	{
		for(int x = 0; x < cell_dim.x; x++)
			for (int y = 0; y < cell_dim.y; y++)
			{
				output_buffer[x][y].char_ = '\000';

				output_buffer[x][y].data =
					current_font->get_char(L'\000');
			}
	}

	void output_manager::set_virtual_screen(
		float width,
		float height
	)
	{
		screen_dim = glm::vec2(width, height);
	}
	
	bool output_manager::set_output_setting(
		uint32_t setting,
		bool enable
	)
	{
		switch (setting)
		{
		case KC_WRAPPING:
			if (enable)
				wrap_setting = &output_manager::wrap;
			else
				wrap_setting = &output_manager::do_bool_nothing;

			break;

		case KC_BUFFER_WRAPPING:
			if (enable)
				buffer_wrap_setting = &output_manager::buffer_wrap;
			else
				buffer_wrap_setting = &output_manager::do_nothing;

			break;

		default:
			return false;
		}

		return true;
	}

	// private methods //
	void output_manager::_generate_cells()
	{
		output_buffer.resize(cell_dim.x);
		for (int x = 0; x < cell_dim.x; x++)
			output_buffer[x].resize(cell_dim.y);
	}

	void output_manager::delete_font()
	{
		if (current_font)
		{
			delete current_font;
			current_font = nullptr;
		}
	}

	void output_manager::delete_program()
	{
		if (program)
		{
			delete program;
			program = nullptr;
		}
	}

	void output_manager::init_members()
	{
		current_font = nullptr;
		program = nullptr;

		_generate_cells();

		// vertex array
		glGenVertexArrays(1, &vertex_array);

		// vertex buffer
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, (cell_dim.x * cell_dim.y) * (sizeof(float) * 4 * (2+2+3)), nullptr, GL_DYNAMIC_DRAW);

		// indicie buffer
		glGenBuffers(1, &indices_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);

		constexpr int vertice_amount = 6;
		uint32_t element_amount = cell_dim.x * cell_dim.y * vertice_amount;
		auto indices = std::make_unique<uint32_t[]>(element_amount);
		uint32_t offset = 0;
		for (size_t i = 0; i < element_amount; i += 6)
		{
			indices[i + 0] = 2 + offset;
			indices[i + 1] = 3 + offset;
			indices[i + 2] = 0 + offset;
			
			indices[i + 3] = 0 + offset;
			indices[i + 4] = 1 + offset;
			indices[i + 5] = 3 + offset;
			offset += 4;
		}

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			element_amount * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

		// how buffer should be read
		glBindVertexArray(vertex_array);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*7, (void*)0); // pos
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*7, (void*)8); // tex
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float)*7,(void*)16); // color
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	// SETTING FUNC //

	bool output_manager::wrap()
	{
		// check if current draw pos x is more then the width of the screen
		if (cur_draw_pos.x >= (screen_dim.x - current_font->widest_glyph))
		{
			cur_draw_pos.y -= current_font->highest_glyph;
			cur_draw_pos.x = 0.0f;

			// no point of calculating something that
			// won't be on screen
			if (cur_draw_pos.y <= 0)
				return true;
		}

		return false;
	}

	void output_manager::buffer_wrap()
	{
		cur_draw_pos.y -= current_font->highest_glyph;
		cur_draw_pos.x = 0.0f;
	}
}