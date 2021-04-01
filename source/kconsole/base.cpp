#include "headers/base.hpp"

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H  

// glew and glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// standard library
#include <iostream>

namespace kconsole
{
	void lib_test()
	{
		std::cout << "lib test for kubic's console\n";

		// freetype
		FT_Library lib;
		int error = FT_Init_FreeType(&lib);
		FT_Done_FreeType(lib);
		std::cout << "freetype2: " << (error == 0 ? "ok" : "error code(" + std::to_string(error) + ")") << '\n';

		// glm
		glm::mat4x4 mat;
		std::cout << "glm: " << "ok" << '\n';

		// opengl/glfw/glew
		error = glfwInit();
		GLFWwindow* window = glfwCreateWindow(100, 100, "libtest", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		std::cout << "glfw: " << (error == GLFW_TRUE ? "ok" : "failed to start/init") << '\n';;

		error = glewInit();
		glfwTerminate();
		std::cout << "glew: " << (error == GLEW_OK ? "ok" : "failed to start/init") << '\n';;
	}
}