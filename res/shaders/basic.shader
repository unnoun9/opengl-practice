#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 colour;
out vec3 color;

void main()
{
   gl_Position = vec4(pos, 1.0);
   color = colour;
}

#shader fragment
#version 330 core

in vec3 color;
out vec4 le_color;

void main()
{
   le_color = vec4(color, 1.0);
}