#include  "kconsole.hpp"
#include "helper.hpp"
#include WIN_INCLUDE
#include <iostream>

int main()
{
	bool good = false;
	kconsole::console console(820, 160, 10, 90, "your all so gay lol", good);

	bool isgood_font = false;
	console->use_font_mtx("./fonts/rri.ttf", isgood_font, 15);

	std::vector<std::string> errors;
	bool isgood_prog = false;
	console->use_program_mtx(errors, "./shaders/vertex.glsl", "./shaders/fragment.glsl", isgood_prog);
 
	console->start();

	if (!good)
	{
		console->end();
		return 10;
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

	wchar_t** buf = kconsole::make_2D_array<wchar_t>(10, 90);
	kconsole::set_2D_wchar_array(buf, 10, 90, L' ');
	console->set_color_mtx(glm::ivec2(0, 0), glm::ivec2(10, 90), glm::vec3(0.5f, 1.0f, 0.2f));

	buf[0][0] = 'H';
	buf[0][1] = 'e';
	buf[0][2] = 'l';
	buf[0][3] = 'l';
	buf[0][4] = 'o';
	buf[0][5] = ' ';
	buf[0][6] = 'W';
	buf[0][7] = 'o';
	buf[0][8] = 'r';
	buf[0][9] = 'l';
	buf[0][10] = 'd';
	buf[0][11] = ' ';
	buf[0][12] = ':';
	buf[0][13] = 'D';

	while (!console->get_done())
	{	

		console->write2D_mtx(buf);
		std::cout << time(0) << '\n';
	}

	kconsole::delete_2D_array<wchar_t>(buf, 10);

	console->end();


	return 0;
}