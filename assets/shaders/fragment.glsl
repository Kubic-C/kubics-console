#version 330 core

in vec2 tex_coords;
in vec3 text_color;

out vec4 color;

uniform sampler2D font_atlas;

void main()
{    
    color =  vec4(text_color, texture(font_atlas, tex_coords).r);
} 