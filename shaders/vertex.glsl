#version 430

layout(location=0) in vec3 position;
layout(location=1) in vec3 color;

uniform mat4 full_transform_mat;

out vec3 the_color;

void main()
{
   gl_Position = full_transform_mat * vec4(position, 1.0);
   the_color = color;
}