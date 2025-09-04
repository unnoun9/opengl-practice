#version 430

in vec3 the_color;
out vec4 da_color;

void main()
{
   da_color = vec4(the_color, 1.0);
}