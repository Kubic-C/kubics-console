#include  "kconsole.hpp"
#include <Windows.h>

int MAIN
{
	bool good = false;
	kconsole::console console(500, 350, 90, 90, "your all so gay lol", good);

	bool isgood_font = false;
	console->use_font_mtx("./fonts/rri.ttf", isgood_font, 1);

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

	console->set_color_mtx(glm::ivec2(0, 0), glm::ivec2(90, 90), glm::vec3(0.5f, 1.0f, 0.0f));

	while (!console->get_done())
	{
		wchar_t** buf = new wchar_t*[90];
		for (int i = 0; i < 90; i++)
			buf[i] = new wchar_t[90];

		for (int i = 0; i < 90; i++)
			for(int k = 0; k < 90; k++)
				buf[i][k] = L'0';

		console->write2D_mtx(buf);

		for (int i = 0; i < 90; i++)
		{
			delete[] buf[i];
		}

		delete[] buf;
	}

	console->end();


	return 0;
}