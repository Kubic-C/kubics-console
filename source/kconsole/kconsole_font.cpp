#include "headers/kconsole_font.hpp"

// ft
#include <ft2build.h>
#include <freetype/freetype.h>

// gl
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// standard
#include <algorithm>
#include <assert.h>

namespace kconsole
{
	// functions //
	char_quad cdtocq(
		char_data& cd,
		glm::vec2& pos
	)
	{
		float x2 = pos.x + static_cast<float>(cd.bearing.x);
		float y2 = pos.y - (static_cast<float>(cd.dim.y) - static_cast<float>(cd.bearing.y));
		float& w = static_cast<float>(cd.dim.x);
		float& h = static_cast<float>(cd.dim.y);

		float right = x2 + w;
		float top = y2 + h;

		char_quad cq;
						// pos       // tex coords                       // color
		cq.vertex[0] = { right, top, cd.tex_coords[0], cd.tex_coords[1], cd.color.r, cd.color.g, cd.color.b };
		cq.vertex[1] = { x2   , top, cd.tex_coords[2], cd.tex_coords[3], cd.color.r, cd.color.g, cd.color.b };
		cq.vertex[2] = { right, y2,  cd.tex_coords[4], cd.tex_coords[5], cd.color.r, cd.color.g, cd.color.b };
		cq.vertex[3] = { x2   , y2,  cd.tex_coords[6], cd.tex_coords[7], cd.color.r, cd.color.g, cd.color.b };

		return cq;
	}

	// constructors //
	font::font()
		: clookup(nullptr), size(0), tex_id(0)
	{
		
	}

	font::~font()
	{
		unload_font();
	}

	font::font(
		const char* font_dir,
		bool& isgood,
		uint32_t font_size,
		uint32_t loading_range
	)
		: clookup(nullptr), size(0), tex_id(0)
	{
		isgood = load_font(font_dir, font_size, loading_range);
	}

	// helper functions //
	void make_texture_atlas(
		FT_Face& face,
		char_data*& clookup,
		uint32_t& highest_glyph,
		uint32_t& widest_glyph,
		uint32_t& size,
		uint32_t& tex,
		uint32_t loading_range
	)
	{
		// get data to make texture atlas
		clookup = new char_data[loading_range];
		size = loading_range;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
		uint32_t atlas_width = 0;
		uint32_t atlas_height = 0;
		for (ichar c = 0; c < loading_range; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				continue;

			atlas_width += face->glyph->bitmap.width;
			atlas_height = std::max(atlas_height, face->glyph->bitmap.rows);
			widest_glyph = std::max(widest_glyph, face->glyph->bitmap.width);
		}

		// make the texture and allocate some space for it
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			atlas_width,
			atlas_height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			nullptr
		);

		// put characters in the texture and add them to the lookup table
		size_t xoffset = 0;
		for (ichar c = 0; c < loading_range; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				continue;

			// put character in the texture
			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				xoffset,
				0,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			// put character data(not including the bitmap) in the lookup table
			float tx = static_cast<float>(xoffset) / atlas_width;
			float norm_width = static_cast<float>(face->glyph->bitmap.width) / atlas_width;
			float norm_height = static_cast<float>(face->glyph->bitmap.rows) / atlas_height;

			clookup[c] =
			{
				tex, // tex_id
				face->glyph->advance.x >> 6, // advance
				glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top), // bearing
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), // dimensions
				glm::vec3(1.0f, 1.0f, 1.0f), // color
				{ // tex_coords
					tx + norm_width , 0,
					tx              , 0,
					tx + norm_width , norm_height,
					tx              , norm_height,
				}
			};

			// set to fill in the next space.
			xoffset += face->glyph->bitmap.width;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		highest_glyph = atlas_height;
	}

	// methods //
	bool font::load_font(
		const char* font_dir,
		uint32_t font_size,
		uint32_t loading_range
	)
	{
		FT_Library lib;
		if (FT_Init_FreeType(&lib) != 0)
			return false;

		// load face from dir
		FT_Face face;
		if (int err = FT_New_Face(lib, font_dir, 0, &face) != 0)
		{
			FT_Done_FreeType(lib);
			return false;
		}

		// load characters
		FT_Set_Pixel_Sizes(face, 0, font_size);
		make_texture_atlas(face, clookup, highest_glyph, widest_glyph, size, tex_id, loading_range);

		// tell user font loading was successful
		FT_Done_Face(face);
		FT_Done_FreeType(lib);
		return true;
	}

	void font::unload_font()
	{
		if (clookup) // if clookup allocated
		{
			delete[] clookup;
			clookup = nullptr;
			glDeleteTextures(1, &tex_id);
			size = 0;
		}
	}

	char_data font::get_char(
		char char_
	)
	{
		assert(clookup != nullptr);
		return clookup[static_cast<ichar>(char_)];
	}

	char_data font::get_char(
		wchar_t char_
	)
	{
		assert(clookup != nullptr);
		return clookup[static_cast<ichar>(char_)];
	}
}