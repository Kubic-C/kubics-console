#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_tex_coords;
layout (location = 2) in vec3 a_color;

out vec2 tex_coords;
out vec3 text_color;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(a_pos, 0.0, 1.0);
    tex_coords = a_tex_coords;
    text_color = a_color;
}