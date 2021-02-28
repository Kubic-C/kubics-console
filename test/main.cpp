#include  "kconsole.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
	bool good = false;
	kconsole::console console(500, 350, 10, 10, "your all so gay lol", good);

	bool isgood_font = false;
	console->use_font_mtx("./fonts/rri.ttf", isgood_font, 20);

	std::vector<std::string> errors;
	bool isgood_prog = false;
	console->use_program_mtx(errors, "./shaders/vertex.glsl", "./shaders/fragment.glsl", isgood_prog);
 
	console->start();

	if (!good)
	{
		std::cout << "window creation failed, exiting...\n";
		console->end();
		return 10;
	}

	if (!isgood_font)
	{
		std::cout << "failed to set font, exiting...\n";
		console->end();
		return 10;
	}

	if (!isgood_prog)
	{
		std::cout << "failed to set program, exiting...\n";

		console->end();
		return 11;
	}
		for (std::string& err : errors)
			std::cout << err + '\0' << '\n';

	console->set_color_mtx(glm::ivec2(6, 6), glm::ivec2(9, 9), glm::vec3(0.5f, 1.0f, 0.0f));

	while (!console->get_done())
	{
		wchar_t** buf = new wchar_t*[10];
		for (int i = 0; i < 10; i++)
			buf[i] = new wchar_t[10];

		for (int i = 0; i < 10; i++)
			for(int k = 0; k < 10; k++)
				buf[i][k] = L'0';

		console->write2D_mtx((wchar_t**)buf);

		std::wstring str;
		console->get_in_buf_mtx(str);
		std::wcout << str << '\n';

		console->get_in_buf_mtx(str);
		std::wcout << str << '\n';

		console->get_in_buf_mtx(str);
		std::wcout << str << '\n';

		console->get_in_buf_mtx(str);
		std::wcout << str << '\n';

		console->get_in_buf_mtx(str);
		std::wcout << str << '\n';

		console->get_in_buf_mtx(str);
		std::wcout << str << '\n';

		for (int i = 0; i < 10; i++)
		{
			delete[] buf[i];
		}

		delete[] buf;
	}

	console->end();

	std::string error = "";
	while ((error = console->get_last_error()).empty() != true)
		std::cout << error << '\n';


	return 0;
}