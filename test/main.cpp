#include  "kconsole.hpp"
#include "helper.hpp"
#include WIN_INCLUDE
#include <iostream>

int MAIN
{
	bool good = false;
	kconsole::console console(820, 160, 10, 90, "kubic's console :D", good);

	bool isgood_font = false;
	console->use_font_mtx("./fonts/rri.ttf", isgood_font, 30);

	std::vector<std::string> errors;
	bool isgood_prog = false;
	console->use_program_mtx(errors, "./shaders/vertex.glsl", "./shaders/fragment.glsl", isgood_prog);
 
	console->start();

	if (!good)
	{
		console->end();
		return 9;
	}

	if (!isgood_font)
	{
		console->end();
		return 10;
	}

	if (!isgood_prog)
	{
		console->end();
		return 11;
	}

	console->set_output_setting_mtx(KC_BUFFER_WRAPPING, false);
	wchar_t** buf = kconsole::make_2D_array<wchar_t>(10, 90);
	kconsole::set_2D_wchar_array(buf, 10, 90, L' ');
	console->set_color_mtx(glm::ivec2(0, 0), glm::ivec2(10, 90), glm::vec3(0.5f, 1.0f, 0.2f));

	while (!console->get_done())
	{	
		std::wstring wstr;
		console->get_in_buf_mtx(wstr);
		kconsole::set_2D_wchar_array(buf, 10, 90, L' ');
		kconsole::print(buf, 10, 90, 0, 0, wstr);

		if (console->key_pressed_mtx(KC_KEY_F2))
		{
			console->clear_output_buffer_mtx();
			console->clear_input_buffer_mtx();
			kconsole::set_2D_wchar_array(buf, 10, 90, L' ');
		}

		console->write2D_mtx(buf);
	}

	kconsole::delete_2D_array<wchar_t>(buf, 10);

	console->end();

	return 0;
}