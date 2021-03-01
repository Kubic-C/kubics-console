// load fonts from file
#pragma once
#ifndef KUBICS_CONSOLE_FONT_LOADING_H
#define KUBICS_CONSOLE_FONT_LOADING_H
#define NOT_CHAR 0;

#include "base.hpp"
#include <glm/glm.hpp>

namespace kconsole
{
	enum { x = 0, y = 1 };

	typedef unsigned int ichar;

	// char_data holds,
	// holds data on how a character
	// should be rendered
	struct char_data
	{
		uint32_t tex_id;
		int advance;
		glm::ivec2 bearing;
		glm::vec2 dim;
		glm::vec3 color;
		float tex_coords[8];
	};

	// char vertex, simlar to char_data
	// but a more raw-like form of it
	struct char_vertex
	{
		float vertices[2];
		float tex_coords[2];
		float color[3]; 
	};

	// char quad
	struct char_quad
	{
		char_vertex vertex[4];
	};

	// char data to char quad
	char_quad cdtocq(
		char_data& cd,
		glm::vec2& pos
	);

	// font class enables fonts to be used
	// in opengl
	class font
	{
	public:
		font();
		~font();

		// load font file
		font(
			const char* font_dir,
			bool& isgood, size_t 
			font_size = 48, 
			uint32_t loading_range = 128
		);

	public:
		// load font file. return true for good and returns false for bad
		bool load_font(
			const char* font_dir, 
			uint32_t font_size = 48,
			uint32_t loading_range = 128
		);

		// unload a font if there is any
		void unload_font();

		// get character data
		char_data get_char(
			char char_
		);

		char_data get_char(
			wchar_t char_
		);

		uint32_t tex_id;
		uint32_t atlas_height = 0;

	private:
		char_data* clookup;
		uint32_t size;
	};
}

#endif // KUBICS_CONSOlE_FONT_LOADING_H