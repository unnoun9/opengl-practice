#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;

out vec2 v_tex_coord; // v stands for varying apparently

uniform mat4 u_projection; // later on, it would be model view projection or mvp matrix

void main()
{
   gl_Position = u_projection * position;
   v_tex_coord = tex_coord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_tex_coord;

uniform vec4 u_color;
uniform sampler2D u_texture;

void main()
{
   color = texture(u_texture, v_tex_coord);;
}