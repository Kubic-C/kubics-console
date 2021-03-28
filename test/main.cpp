#include  "kconsole.hpp"
#include "helper.hpp"
#include WIN_INCLUDE
#include <iostream>

int main()
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
	console->set_output_setting_mtx(KC_WRAPPING, true);
	wchar_t** buf = kconsole::make_2D_array<wchar_t>(10, 90);
	console->set_color_mtx(glm::ivec2(0, 0), glm::ivec2(10, 90), glm::vec3(1.0f, 1.0f, 1.0f));
	kconsole::set_2D_wchar_array(buf, 10, 90, L' ');

	kconsole::set_2D_wchar_array(buf, 10, 90, L' ');
	kconsole::print(buf, 10, 90, 0, 0, L"Hello World!");

	int i = 0;
	while (!console->get_done())
	{
		console->set_color_mtx(glm::ivec2(0, 0), glm::ivec2(1, 1), glm::vec3(sin(time(0)), 1.0f, 0.2f));
		std::wstring wstr;

		console->get_in_buf_mtx(wstr);
		kconsole::print(buf, 10, 90, 0, 0,wstr);

		console->write2D_mtx(buf);
		kconsole::set_2D_wchar_array(buf, 10, 90, L' '); 
	}

	kconsole::delete_2D_array<wchar_t>(buf, 10);

	console->end();

	return 0;
}